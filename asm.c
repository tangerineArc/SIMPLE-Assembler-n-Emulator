#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "./utils.c"

VectorStr sourceCode;
VectorPairIntStr errors;
MapStringToPairStrInt instructionSet;

/******************************
    generate error messages    
******************************/
void raiseError(int lineNumber, char* message) {
    char* lineStr = (char*) malloc(24);
    char* errorMessage = (char*) malloc(256);

    lineStr[0] = '\0';
    sprintf(lineStr, "%d", lineNumber);

    errorMessage[0] = '\0';
    strcat(errorMessage, "Line ");
    strcat(errorMessage, lineStr);
    strcat(errorMessage, " >>> ERROR: ");
    strcat(errorMessage, message);

    VectorPairIntStr_Push(&errors, lineNumber, errorMessage);
}

/***************************************
    formats source code line-by-line    
***************************************/
char* trim(char* line, int size, int lineNumber) {
    char* trimmedStr = (char*) malloc(size);

    int spaces = 0;

    int i;

    for (i = 0; i < 2; i ++) {
        strrev(line);
        while (size > 0 && (line[size - 1] == ' ' || line[size - 1] == '\t')) {
            line[size - 1] = '\0';
            size --;
        }
    }

    trimmedStr[0] = '\0';
    for (i = 0; i < size; i ++) {
        int j = i;

        if (line[i] == ';') {
            break;
        }

        if (line[i] == ':') {
            strcat(trimmedStr, ":");
            if (i == size - 1 || line[i + 1] != ' ') {
                strcat(trimmedStr, " ");
            }
            continue;
        }

        if (line[i] != ' ' && line[i] != '\t') {
            strncat(trimmedStr, &line[i], 1);
            continue;
        }

        strcat(trimmedStr, " ");

        while (line[i] == line[j] && j < size) {
            j ++;
        }
        i = j - 1;
    }

    while (strlen(trimmedStr) > 0 && (trimmedStr[strlen(trimmedStr) - 1] == ' ' || trimmedStr[strlen(trimmedStr) - 1] == '\t')) {
        trimmedStr[strlen(trimmedStr) - 1] = '\0';
    }
    
    for (i = 0; (unsigned) i < strlen(trimmedStr); i ++) {
        if (trimmedStr[i] == ' ') {
            spaces ++;
        }
    }

    if (spaces > 2) {
        raiseError(lineNumber, "incorrect spacing");
    }

    return trimmedStr;
}

/**********************************************************
    generates a map of pnemonic : (opcode, numOperands)    
**********************************************************/
void initializeInstructionSet() {
    MapStringToPairStrInt_Initialize(&instructionSet);

    MapStringToPairStrInt_Add(&instructionSet, "data", "", 1);
    MapStringToPairStrInt_Add(&instructionSet, "ldc", "00", 1);
    MapStringToPairStrInt_Add(&instructionSet, "adc", "01", 1);
    MapStringToPairStrInt_Add(&instructionSet, "ldl", "02", 2);
    MapStringToPairStrInt_Add(&instructionSet, "stl", "03", 2);
    MapStringToPairStrInt_Add(&instructionSet, "ldnl", "04", 2);
    MapStringToPairStrInt_Add(&instructionSet, "stnl", "05", 2);
    MapStringToPairStrInt_Add(&instructionSet, "add", "06", 0);
    MapStringToPairStrInt_Add(&instructionSet, "sub", "07", 0);
    MapStringToPairStrInt_Add(&instructionSet, "shl", "08", 0);
    MapStringToPairStrInt_Add(&instructionSet, "shr", "09", 0);
    MapStringToPairStrInt_Add(&instructionSet, "adj", "0A", 1);
    MapStringToPairStrInt_Add(&instructionSet, "a2sp", "0B", 0);
    MapStringToPairStrInt_Add(&instructionSet, "sp2a", "0C", 0);
    MapStringToPairStrInt_Add(&instructionSet, "call", "0D", 2);
    MapStringToPairStrInt_Add(&instructionSet, "return", "0E", 0);
    MapStringToPairStrInt_Add(&instructionSet, "brz", "0F", 2);
    MapStringToPairStrInt_Add(&instructionSet, "brlz", "10", 2);
    MapStringToPairStrInt_Add(&instructionSet, "br", "11", 2);
    MapStringToPairStrInt_Add(&instructionSet, "HALT", "12", 0);
    MapStringToPairStrInt_Add(&instructionSet, "SET", "", 1);
}

/****************************************************
    executes the first pass of the assembly cycle    
****************************************************/
void executePass1(char* sourceFilePath) {
    FILE* sourceFile = fopen(sourceFilePath, "r");

    char* line = NULL;
    int lineNumber = 0;

    if (sourceFile == NULL) {
        fprintf(stderr, "ASSEMBLER_ERROR: Could not open file \"%s\"\n", sourceFilePath);
        exit(-2);
    }
    
    printf("Sucessfully loaded \"%s\"\n", sourceFilePath);

    VectorStr_Initialize(&sourceCode);
    VectorPairIntStr_Initialize(&errors);
    
    while ((line = readLine(sourceFile)) != NULL) {
        char* tmp = trim(line, strlen(line), ++ lineNumber);
        if (strlen(tmp) > 0) {
            VectorStr_Push(&sourceCode, tmp);
        }
    }

    fclose(sourceFile);

    initializeInstructionSet();
}

int main(int argc, char* argv[]) {
    int i;

    PairStrInt* res;

    if (argc != 2) {
        fprintf(stderr, "ASSEMBLER_ERROR: Usage: ./a <source-file-path>\n");
        exit(-1);
    }

    executePass1(argv[1]);

    /********** for testing **********/
    for (i = 0; i < sourceCode.size; i ++) {
        printf("%s\n", VectorStr_Get(&sourceCode, i));
    }

    for (i = 0; (unsigned) i < errors.size; i ++) {
        printf("\n%d %s\n", errors.data[i].first, errors.data[i].second);
    }

    res = MapStringToPairStrInt_Find(&instructionSet, "return");
    if (res) {
        printf("Found: %s %d\n", res->first, res->second);
    } else {
        printf("Not found\n");
    }
    /********** testing ends **********/

    VectorStr_Clear(&sourceCode);
    VectorPairIntStr_Clear(&errors);
    MapStringToPairStrInt_Clear(&instructionSet);

    return 0;
}