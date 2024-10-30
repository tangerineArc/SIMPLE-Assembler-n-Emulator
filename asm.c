#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "./utils.c"

VectorStr sourceCode;
VectorPairIntStr errors;
MapStrToPairStrInt instructionSet;
MapStrToInt labels;

/*
 * TO-DO: FIX parseLabels FUNCTION 
 */

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

/************************************************************
    generates a map of {pnemonic : (opcode, numOperands)}    
************************************************************/
void initializeInstructionSet() {
    MapStrToPairStrInt_Initialize(&instructionSet);

    MapStrToPairStrInt_Add(&instructionSet, "data",     "", 1);
    MapStrToPairStrInt_Add(&instructionSet, "ldc",    "00", 1);
    MapStrToPairStrInt_Add(&instructionSet, "adc",    "01", 1);
    MapStrToPairStrInt_Add(&instructionSet, "ldl",    "02", 2);
    MapStrToPairStrInt_Add(&instructionSet, "stl",    "03", 2);
    MapStrToPairStrInt_Add(&instructionSet, "ldnl",   "04", 2);
    MapStrToPairStrInt_Add(&instructionSet, "stnl",   "05", 2);
    MapStrToPairStrInt_Add(&instructionSet, "add",    "06", 0);
    MapStrToPairStrInt_Add(&instructionSet, "sub",    "07", 0);
    MapStrToPairStrInt_Add(&instructionSet, "shl",    "08", 0);
    MapStrToPairStrInt_Add(&instructionSet, "shr",    "09", 0);
    MapStrToPairStrInt_Add(&instructionSet, "adj",    "0A", 1);
    MapStrToPairStrInt_Add(&instructionSet, "a2sp",   "0B", 0);
    MapStrToPairStrInt_Add(&instructionSet, "sp2a",   "0C", 0);
    MapStrToPairStrInt_Add(&instructionSet, "call",   "0D", 2);
    MapStrToPairStrInt_Add(&instructionSet, "return", "0E", 0);
    MapStrToPairStrInt_Add(&instructionSet, "brz",    "0F", 2);
    MapStrToPairStrInt_Add(&instructionSet, "brlz",   "10", 2);
    MapStrToPairStrInt_Add(&instructionSet, "br",     "11", 2);
    MapStrToPairStrInt_Add(&instructionSet, "HALT",   "12", 0);
    MapStrToPairStrInt_Add(&instructionSet, "SET",      "", 1);
}

/*********************************************
    checks whether the label name is valid    
*********************************************/
bool checkLabelIdentifierValidity(char* label) {
    unsigned int i;
    for (i = 0; i < strlen(label); i ++) {
        if ((label[i] >= 'a' && label[i] <= 'z') || (label[i] >= 'A' && label[i] <= 'Z') || (label[i] >= '0' && label[i] <= '9') || label[i] == '_') {
            continue;
        }
        return false;
    }
    if ((label[0] >= 'a' && label[0] <= 'z') || (label[0] >= 'A' && label[0] <= 'Z') || label[0] == '_') {
        return true;
    }
    return false;
}

/***********************************************************
    generates a map of {labels : positon} of declaration    
***********************************************************/
void parseLabels() {
    int i;

    MapStrToInt_Initialize(&labels);

    for (i = 0; i < sourceCode.size; i ++) {
        char* label = (char*) malloc(strlen(sourceCode.data[i] + 1));

        unsigned int j;

        label[0] = '\0';
        for (j = 0; (unsigned) j < strlen(sourceCode.data[i]); j ++) {
            if (sourceCode.data[i][j] == ':') {
                if (!checkLabelIdentifierValidity(label)) {
                    raiseError(i + 1, "invalid label identifier");
                    break;
                }

                if (MapStrToInt_Find(&labels, label)) {
                    char* errorMessage = (char*) malloc(256);

                    if (strlen(sourceCode.data[i]) > j + 4 && !strcmp(substr(sourceCode.data[i], j + 2, 3), "SET")) {
                        continue;
                    }

                    if (strlen(sourceCode.data[i]) > j + 5 && !strcmp(substr(sourceCode.data[i], j + 2, 4), "data") && *MapStrToInt_Find(&labels, label) < 0) {
                        MapStrToInt_Add(&labels, label, i);
                        continue;
                    }

                    errorMessage[0] = '\0';
                    strcat(errorMessage, "multiple declarations found for label: ");
                    strcat(errorMessage, label);
                    raiseError(i + 1, errorMessage);
                    free(errorMessage);
                }
                
                if (strlen(sourceCode.data[i]) > j + 4 && !strcmp(substr(sourceCode.data[i], j + 2, 3), "SET")) {
                    int* tmp = MapStrToInt_Find(&labels, label);
                    *tmp = -i;
                    continue;
                }
                MapStrToInt_Add(&labels, label, i);
                break;
            }
            strncat(label, &sourceCode.data[i][j], 1);
        }
    }
}

/****************************************************
    executes the first pass of the assembly cycle    
****************************************************/
void executePass1(char* sourceFilePath) {
    FILE* sourceFile = fopen(sourceFilePath, "r");

    char* line = NULL;
    int lineNumber = 0;

    if (sourceFile == NULL) {
        fprintf(stderr, "ASSEMBLER_ERROR: could not open file \"%s\"\n", sourceFilePath);
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

    parseLabels();
}

int main(int argc, char* argv[]) {
    /********** for testing **********/
    int i;

    PairStrInt* res;
    int* lol;
    /********** testing ends **********/

    if (argc != 2) {
        fprintf(stderr, "ASSEMBLER_ERROR: Usage: ./a <source-file-path>\n");
        exit(-1);
    }

    executePass1(argv[1]);

    /********** for testing **********/
    for (i = 0; i < sourceCode.size; i ++) {
        printf("%s\n", sourceCode.data[i]);
    }

    for (i = 0; (unsigned) i < errors.size; i ++) {
        printf("\n%d %s\n", errors.data[i].first, errors.data[i].second);
    }

    res = MapStrToPairStrInt_Find(&instructionSet, "ldc");
    if (res) {
        printf("Found: %s %d\n", res->first, res->second);
    } else {
        printf("Not found\n");
    }

    lol = MapStrToInt_Find(&labels, "label");;
    if (lol) {
        printf("Found: %d\n", *lol);
    } else {
        printf("Not found\n");
    }
    /********** testing ends **********/

    VectorStr_Clear(&sourceCode);
    VectorPairIntStr_Clear(&errors);
    MapStrToPairStrInt_Clear(&instructionSet);
    MapStrToInt_Clear(&labels);

    return 0;
}