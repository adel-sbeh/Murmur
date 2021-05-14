#include "linker.h"
#include "import.h"

FileInfo* chngFileCol(FileInfo *tmp, int col){

    tmp->currCol = col;

    tmp->currLineCon = getStrPrt(tmp->currLineCon, tmp->currCol - 1, strlen(tmp->currLineCon), 0);

    return tmp;

}

#include "comments.h"

void ppcRead(FileInfo *fileInf, FILE *desFilePtr){

    fileInf->mode;
    fileInf->isFull;
    fileInf->currLineCon;
    fileInf->currOLineCon;
    fileInf->path;
    fileInf->filePtr;
    fileInf->fileStrPtr;
    fileInf->currLine;
    fileInf->currCol;
    fileInf->nextCol;

    do {

        if(!isStrEmpty(fileInf->currLineCon))
            fileInf = chkForCom(fileInf); //Remove the comments

        if(!isStrEmpty(fileInf->currLineCon))
            fileInf = chkForPprFunc(fileInf); //Check for the preprocessor functions

        //fileInf->currLineCon;

        if(!isStrEmpty(fileInf->currLineCon))
            fprintf(desFilePtr, "@[%d,%d]%s", fileInf->currLine, fileInf->currCol, fileInf->currLineCon);

        printf("\n[Debug]nextCol: %d, currCol: %d, OLen: %d, Line: %d\n", fileInf->nextCol, fileInf->currCol, strlen(fileInf->currOLineCon), fileInf->currLine);

        if(fileInf->nextCol == fileInf->currCol || fileInf->nextCol >= strlen(fileInf->currOLineCon) - 1){ //Get next line only if the column is still set to 1 or if it's set to the last column in the current line

            fgets(fileInf->currOLineCon, MAX_LINE_LENGTH, fileInf->filePtr); //Move to the next line!

            strcpy(fileInf->currLineCon, fileInf->currOLineCon);

            fileInf->currLine++;

            fileInf->currCol = 1;
            fileInf->nextCol = 1;

        }else{

            //Move the column content!
            fileInf->currLineCon = getStrPrt(fileInf->currOLineCon, fileInf->nextCol - 1, strlen(fileInf->currOLineCon), 0);

            fileInf->currCol = fileInf->nextCol;

        }

    } while(!feof(fileInf->filePtr));

}