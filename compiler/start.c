#include "envi.h"
#include "predef.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void Deb(){ //Minimal debug

    printf("[Debug] ");
    SYSTEM_COMMANDS_PAUSE;

}

#include "libraries/types/String.h"
#include "libraries/files/general.h"
#include "extrtyp.h"
#include "libraries/files/gen.c"
#include "libraries/paths/paths.h"

#include <math.h>
#include "libraries/hex/hex.h"

#include "workstation.c"
#include "errors/report.h"

#include "debug.c"
#include "log.c"

void preprocess(FILE *filePtr, char *path, int isFull, TmpFileStruc desFileObj);

int main(int argc, char *argv[]){

    RegDebStr(); //Start the debugging timer

    srand(time(NULL)); //Randomise the number generator output

    Debug("Starting the compiler", 0);

    char path[256];
    char *pathPtr = path;

    if (argc > 1){

        strcpy(path, argv[1]);

        path[strlen(path)] = '\0'; //Make sure a string-end char is always at the end of this environment variable

    }else{

        fgets(path, 255, stdin);

        if(path[0] != '\0' && strlen(path) > 1)
            path[strcspn(path, "\n")] = '\0'; //Make sure to replace the new line char with a string-end char
        else
            exit(-1); //Change the exit code

    }

    //NOTE: you need to check the path format!

    //Get the path of the folder that contains the current file

    DebugWithPath("Received the path: ", path, 0);

    //Start a workstation
    setupWrkstn(pathPtr);

    printf("\n Workstation Path: \"%s\"\nWorkstation Name: \"%s\"\n", wrkstn.Path, wrkstn.Name);

    //Start logging the session
    char *tmpStrPrt1 = apdStr(wrkstn.Name, ".log"), *tmpStrPrt2 = apdStr(wrkstn.Path, tmpStrPrt1);
    newLogFile(tmpStrPrt2);

    free(tmpStrPrt2);
    free(tmpStrPrt1);

    writeLogLine("Compiler Manager", 0, "Testing the `writeLogLine` function", 0, 0, 0);

    FILE *mainFilePtr = OpnStrm(path);

    char *tmpFilePath = apdStr(wrkstn.Path, wrkstn.Name);

    TmpFileStruc tmpFileObj = genFilStr(tmpFilePath); //Create a temporary file for the compiling process
    //There should be only one file per process
    //Note: the generated files will get deleted automatically in the future, unless the 'OUTPUT_TEMP_FILE' environment variable is set to 'true'/'1'

    writeLogLine("Compiler Manager", 0, "Starting the preprocessor.", 0, 0, 0);
    preprocess(mainFilePtr, path, 1, tmpFileObj); //Initiate the compiling process

    /*if(mainFilePtr != NULL)
        fclose(mainFilePtr);*/

    /*if(tmpFileObj.ptr != NULL)
        fclose(tmpFileObj.ptr);*/

    writeLogLine("Compiler Manager", 0, "Closed all files sessions.", 0, 0, 0);

    endWrkstn();

    writeLogLine("Compiler Manager", 0, "Closed the current workstation.", 0, 0, 0);

    RegDebEnd(); //End the debugging timer

    Deb();

    //exit(EXIT_SUCCESS);

    writeLogLine("Compiler Manager", 0, "Task finished successfully! (Ending process)", 0, 0, 0);

    clsLogSec();

    return 0;

}

#include "preprocessor/checker.h"
#include "preprocessor/reader.h"
#include "skip.h"

#include "c/compile.c"

#include "lexical/lex.h"

#include "parser/par.h"

void CProcess(FileInfo *fileInf, TmpFileStruc desFileObj);

void preprocess(FILE *filePtr, char *path, int isFull, TmpFileStruc desFileObj){ //Compile a file and it's content

    writeLogLine("Preprocessor", 0, "Preprocessor started!", 0, 0, 0);

    //FILE *desFileStrPtr = desFileObj.ptr; //Save the start pointer!

    //desFilePtr; //This file should be used to store all the code that will be compiled in the process

    FileInfo *fileInf = checkFlags(filePtr, path, isFull); //An object that contains the file info!

    printf("\n[Debug] Mode: %c\n", fileInf->mode);
    printf("[Debug] Is Full: %d\n", fileInf->isFull);
    printf("[Debug] Current Line Content: %s\n", fileInf->currLineCon);
    printf("[Debug] Current Line Original Content: %s\n", fileInf->currOLineCon);
    printf("[Debug] Path: %s\n", fileInf->path);

    if(fileInf->mode == 'U'){ //This is neither a `.mur` file nor a `.lib.mur` file

        writeLogLine("Compiler Manager", 2, "Unknown input file extension!", 0, 0, 0);

        exit(-1); //Exit! Don't worry about the allocated memory, the system is gonna clean it up.

    }else{

        crtDatFil(); //Create a ".dat" file
        opnRptFil(); //Create a ".opf" file

        ppcRead(fileInf, desFileObj.ptr, "<main>"); //Let the preprocessor do its thing!
                                        //^ change this value to the
                                        //full path of the main file

        //fclose(tmpDatCtr.ptr);
        clsDatFil(); //Save & close the data file (so you can use its data later)
        fclose(filePtr); //Close the main input file stream

        fclose(desFileObj.ptr);

        char *tmpPthStr = apdStr(desFileObj.pth, ".tmp");
        desFileObj.ptr = fopen(tmpPthStr, "r"); //Switch the file mode to "read mode"

        free(tmpPthStr);

        FILE* lexFil = lexProc(desFileObj); //Start the lexical-processing process!

        PrsProc(desFileObj, lexFil); //Start parsing the generated tokens!

        free(desFileObj.pth);
        fclose(desFileObj.ptr);
        freeDatFil(); //Free the data file pointer!

        rpt(REPORT_CODE_MESSAGE, REPORT_SECTION_GENERAL, "This process is about to end", "<main>", 0, 0);

        clsRptFil(); //Close the ".opf" file

        //(FILE *filePtr, char *path, int isFull, TmpFileStruc desFileObj)

        /* //[START] Start the C processor

        FILE *svPtr = desFileObj.ptr;

        movCnt(fileInf, desFileObj.ptr);

        desFileObj.ptr = svPtr;

        CProcess(fileInf, desFileObj);

        //[END] Start the C processor */

    }

}

void CProcess(FileInfo *fileInf, TmpFileStruc tmpFileObj){

    if(OUTPUT_EXECUTABLE){

        char *cFilPth = apdStr(tmpFileObj.pth, ".c");
        FILE *cFilPtr = fopen(cFilPth, "w");

        fclose(tmpFileObj.ptr);

        tmpFileObj.ptr = fopen(apdStr(tmpFileObj.pth, ".tmp"), "r");

        cpyFileCon(cFilPtr, tmpFileObj.ptr);

        fclose(cFilPtr);

        cmplCCode(cFilPth); //Generate the binary!

    }

}