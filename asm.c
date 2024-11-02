#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "./headers/dataStructures.h"
#include "./headers/utils.h"

VectorStr sourceCode;
VectorPairIntStr errors;
MapStrToPairStrInt instructionSet;
MapStrToInt labels;
VectorListingCustom sourceTable;
VectorInt programCounter;

bool isHaltPresent = false;

/******************************
    generate error messages    
******************************/
void raiseError(int lineNumber, char* message) {
    char* lineStr = (char*) malloc(24);
    char* errorMessage = (char*) malloc(256);

    lineStr[0] = '\0';
    sprintf(lineStr, "%d", lineNumber + 1);

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
char* trim(char* line, int lineNumber) {
    char* trimmedStr = (char*) malloc(strlen(line));

    int spaces = 0;

    size_t i;

    for (i = 0; i < 2; i ++) {
        strrev(line);
        while (strlen(line) > 0 && (line[strlen(line) - 1] == ' ' || line[strlen(line) - 1] == '\t')) {
            line[strlen(line) - 1] = '\0';
        }
    }

    trimmedStr[0] = '\0';
    for (i = 0; i < strlen(line); i ++) {
        int j = i;

        if (line[i] == ';') {
            break;
        }

        if (line[i] == ':') {
            strcat(trimmedStr, ":");
            if (i == strlen(line) - 1 || line[i + 1] != ' ') {
                strcat(trimmedStr, " ");
            }
            continue;
        }

        if (line[i] != ' ' && line[i] != '\t') {
            strncat(trimmedStr, &line[i], 1);
            continue;
        }

        strcat(trimmedStr, " ");

        while (line[i] == line[j] && (size_t) j < strlen(line)) {
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
        raiseError(lineNumber + 1, "incorrect spacing");
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
        char* label = (char*) malloc(strlen(sourceCode.data[i]) + 1);

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
                    MapStrToInt_Add(&labels, label, -i);
                    continue;
                }
                MapStrToInt_Add(&labels, label, i);
                break;
            }
            strncat(label, &sourceCode.data[i][j], 1);
        }
    }
}

/***********************************************************************************************
    modifies sourceCode vector by translating pseudo-instructions (SET) to real-instructions    
***********************************************************************************************/
void translatePseudoInstructions() {
    VectorStr realInstructionsSourceCode;

    int i;
    VectorStr_Initialize(&realInstructionsSourceCode);
    for (i = 0; i < sourceCode.size; i ++) {
        char* token = (char*) malloc(strlen(sourceCode.data[i]) + 1);
        bool state = false;

        long unsigned int j;
        token[0] = '\0';
        for (j = 0; j < strlen(sourceCode.data[i]); j ++) {
            strncat(token, &sourceCode.data[i][j], 1);
            
            if (sourceCode.data[i][j] == ':') {
                if (strlen(token) > 0) {
                    token[strlen(token) - 1] = '\0';
                }

                if (strlen(sourceCode.data[i]) > j + 5 && !strcmp(substr(sourceCode.data[i], j + 2, 3), "SET")) {
                    state = true;
                    if (abs(*MapStrToInt_Find(&labels, token)) == i) {
                        char* info = (char*) malloc(256);
                        info[0] = '\0';

                        MapStrToInt_Add(&labels, token, realInstructionsSourceCode.size - 1);

                        strcat(info, substr(sourceCode.data[i], 0, j + 1));
                        strcat(info, " data ");
                        strcat(info, substr(sourceCode.data[i], j + 6, strlen(sourceCode.data[i]) - (j + 6)));
                        VectorStr_Push(&realInstructionsSourceCode, info);
                        free(info);
                    } else {
                        char* _t1 = (char*) malloc(256);
                        char* _t2 = (char*) malloc(256);

                        if (strlen(sourceCode.data[i]) <= j + 5) {
                            free(_t1);
                            free(_t2);
                            break;
                        }

                        VectorStr_Push(&realInstructionsSourceCode, "adj 10000");
                        VectorStr_Push(&realInstructionsSourceCode, "stl -1");
                        VectorStr_Push(&realInstructionsSourceCode, "stl 0");
                        
                        _t1[0] = '\0';
                        strcat(_t1, "ldc ");
                        strcat(_t1, substr(sourceCode.data[i], j + 6, strlen(sourceCode.data[i]) - (j + 6)));
                        VectorStr_Push(&realInstructionsSourceCode, _t1);
                        free(_t1);

                        _t2[0] = '\0';
                        strcat(_t2, "ldc ");
                        strcat(_t2, substr(token, 0, j));
                        VectorStr_Push(&realInstructionsSourceCode, _t2);
                        free(_t2);

                        VectorStr_Push(&realInstructionsSourceCode, "stnl 0");
                        VectorStr_Push(&realInstructionsSourceCode, "ldl 0");
                        VectorStr_Push(&realInstructionsSourceCode, "ldl -1");
                        VectorStr_Push(&realInstructionsSourceCode, "adj -1000");
                    }
                    break;
                }
            }
        }

        if (!state && strlen(sourceCode.data[i]) != 0) {
            VectorStr_Push(&realInstructionsSourceCode, sourceCode.data[i]);
        }

        free(token);
    }

    sourceCode = realInstructionsSourceCode;
}

/**************************************************************
    separates data segments from instructions in sourceCode    
**************************************************************/
void separateDataFromInstructions() {
    VectorStr properlySeparatedSoureCode;
    VectorStr dataSegments;

    int i;

    VectorStr_Initialize(&properlySeparatedSoureCode);
    VectorStr_Initialize(&dataSegments);
    for (i = 0; i < sourceCode.size; i ++) {
        bool state = false;
        unsigned int j;
        for (j = 0; j < strlen(sourceCode.data[i]); j ++) {
            if (!strcmp(substr(sourceCode.data[i], j, 4), "data") && j + 4 < strlen(sourceCode.data[i])) {
                VectorStr_Push(&dataSegments, sourceCode.data[i]);
                state = true;
                break;
            }

            if (sourceCode.data[i][strlen(sourceCode.data[i]) - 1] == ':' && i + 1 < sourceCode.size && !strcmp(substr(sourceCode.data[i + 1], 0, 4), "data")) {
                VectorStr_Push(&dataSegments, sourceCode.data[i]);
                state = true;
                break;
            }
        }
        if (!state) {
            VectorStr_Push(&properlySeparatedSoureCode, sourceCode.data[i]);
        }
    }

    for (i = 0; i < dataSegments.size; i ++) {
        VectorStr_Push(&properlySeparatedSoureCode, dataSegments.data[i]);
    }
    sourceCode = properlySeparatedSoureCode;
}

/***********************************************************
    returns whether a string represents a decimal number    
***********************************************************/
bool isDecimal(char* s) {
    bool verdict = true;
    unsigned int i;
    for (i = 0; i < strlen(s); i ++) {
        verdict &= (s[i] >= '0' && s[i] <= '9');
    }
    return verdict;
}

/**********************************************************
    returns whether a string represents an octal number    
**********************************************************/
bool isOctal(char* s) {
    bool verdict = true;

    unsigned int i;

    if (strlen(s) < 3) {
        return false;
    }
    
    for (i = 2; i < strlen(s); i ++) {
        verdict &= (s[i] >= '0' && s[i] <= '7');
    }
    return verdict & (s[0] == '0' && (s[1] == 'o' || s[1] == 'O'));
}

/***************************************************************
    returns whether a string represents a hexadecimal number    
***************************************************************/
bool isHexadecimal(char* s) {
    bool verdict = true;

    unsigned int i;

    if (strlen(s) < 3) {
        return false;
    }

    for (i = 2; i < strlen(s); i ++) {
        verdict &= ((s[i] >= '0' && s[i] <= '9') || (s[i] >= 'a' && s[i] <= 'f') || (s[i] >= 'A' && s[i] <= 'F'));
    }
    return verdict & (s[0] == '0' && (s[1] == 'x' || s[1] == 'X'));

}

/*********************************************************************************
    returns whether the operand is a label / hex value / oct value / dec value    
*********************************************************************************/
int getOperandType(char* operand) {
    if (strlen(operand) == 0) {
        return 0;
    }

    if (operand[0] == '+' || operand[0] == '-') {
        strrev(operand);
        operand[strlen(operand) - 1] = '\0';
        strrev(operand);
    }

    if (strlen(operand) == 0) {
        return -1;
    } else if (isDecimal(operand)) {
        return 10;
    } else if (isOctal(operand)) {
        return 8;
    } else if (isHexadecimal(operand)) {
        return 16;
    } else if (checkLabelIdentifierValidity(operand)) {
        return 1;
    }
    return -1;
}

/*********************************************************************************************************
    parses the sourceCode and stores label, mnemonic, operand, operandType in VectorListingCustom data    
*********************************************************************************************************/
void tabulateSourceCode() {
    int PC = 0;

    int i;
    for (i = 0; i < sourceCode.size; i ++) {
        VectorStr row;

        char* curr = (char*) malloc(strlen(sourceCode.data[i]) * 2);

        int ptr = 1;

        unsigned int j;

        int opType;

        curr[0] = '\0';

        VectorStr_Initialize(&row);
        for (j = 0; j < 4; j ++) VectorStr_Push(&row, "");
        VectorStr_Resize(&row, 10);

        for (j = 0; j < strlen(sourceCode.data[i]); j ++) {
            if (sourceCode.data[i][j] == ':') {
                VectorStr_Insert(&row, 0, curr);
                curr[0] = '\0';

                j ++;
                continue;
            } else if (sourceCode.data[i][j] == ' ') {
                VectorStr_Insert(&row, ptr ++, curr);
                curr[0] = '\0';

                continue;
            }
            strncat(curr, &sourceCode.data[i][j], 1);
            if (j == strlen(sourceCode.data[i]) - 1) {
                VectorStr_Insert(&row, ptr ++, curr);
            }
        }
        
        if (strlen(row.data[1]) != 0) {
            bool tmp = true;
            VectorListingCustom_Insert(&sourceTable, i, IS_LABEL_PRESENT, &tmp);
        } else {
            bool tmp = false;
            VectorListingCustom_Insert(&sourceTable, i, IS_LABEL_PRESENT, &tmp);
        }
        
        if (!strcmp(row.data[1], "HALT")) {
            isHaltPresent = true;
        }
        
        if (strlen(row.data[0]) != 0) {
            MapStrToInt_Add(&labels, row.data[0], PC);
        }
        
        VectorInt_Insert(&programCounter, i, PC);
        
        if (ptr == 1) {
            int val = 0;
            VectorListingCustom_Insert(&sourceTable, i, LABEL, row.data[0]);
            VectorListingCustom_Insert(&sourceTable, i, MNEMONIC, "");
            VectorListingCustom_Insert(&sourceTable, i, OPERAND, "");
            VectorListingCustom_Insert(&sourceTable, i, OPERAND_TYPE, &val);
            
            VectorStr_Clear(&row);
            free(curr);
            continue;
        }
        PC ++;
        
        if (MapStrToPairStrInt_Find(&instructionSet, row.data[1]) == NULL) {
            raiseError(i + 1, "invalid mnemonic");

            VectorStr_Clear(&row);
            free(curr);
            continue;
        }
        
        if (minOf2Ints(MapStrToPairStrInt_Find(&instructionSet, row.data[1])->second, 1) != ptr - 2) {
            raiseError(i + 1, "invalid OPCode-syntax combination");

            VectorStr_Clear(&row);
            free(curr);
            continue;
        }
        
        VectorListingCustom_Insert(&sourceTable, i, LABEL, row.data[0]);    
        VectorListingCustom_Insert(&sourceTable, i, MNEMONIC, row.data[1]);
        VectorListingCustom_Insert(&sourceTable, i, OPERAND, row.data[2]);
        opType = getOperandType(row.data[2]);
        VectorListingCustom_Insert(&sourceTable, i, OPERAND_TYPE, &opType);
        if (sourceTable.data[i].operandType == 1 && MapStrToInt_Find(&labels, sourceTable.data[i].operand) == NULL) {
            raiseError(i + 1, "no such label");
        } else if (sourceTable.data[i].operandType == -1) {
            raiseError(i + 1, "invalid number");
        }

        free(curr);
        VectorStr_Clear(&row);
    }
}

/****************************************************
    executes the first pass of the assembly cycle    
****************************************************/
void executePass1(char* sourceFilePath) {
    FILE* sourceFile = fopen(sourceFilePath, "r");

    char* line = NULL;

    if (sourceFile == NULL) {
        fprintf(stderr, "ASSEMBLER_ERROR: could not open file \"%s\"\n", sourceFilePath);
        exit(-2);
    }
    
    printf("Sucessfully loaded \"%s\"\n", sourceFilePath);

    VectorStr_Initialize(&sourceCode);
    VectorPairIntStr_Initialize(&errors);
    
    while ((line = readLine(sourceFile)) != NULL) {
        char* tmp = trim(line, sourceCode.size);
        VectorStr_Push(&sourceCode, tmp);
    }

    fclose(sourceFile);

    initializeInstructionSet();
    
    parseLabels();

    if (errors.size == 0) {
        translatePseudoInstructions();
    }

    VectorListingCustom_Initialize(&sourceTable);
    VectorListingCustom_Resize(&sourceTable, sourceCode.size);

    VectorInt_Initialize(&programCounter);
    VectorInt_Resize(&programCounter, sourceCode.size);

    separateDataFromInstructions();

    tabulateSourceCode();
}

int main(int argc, char* argv[]) {
    /********** for testing **********/
    int i;
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
    printf("\n");
    for (i = 0; i < sourceTable.size; i ++) {
        printf("%s %s %s %d %d\n", sourceTable.data[i].label, sourceTable.data[i].mnemonic, sourceTable.data[i].operand, sourceTable.data[i].operandType, sourceTable.data[i].isLabelPresent);
    }
    printf("\n");
    for (i = 0; i < programCounter.size; i ++) {
        printf("%d\n", programCounter.data[i]);
    }
    /*
    for (i = 0; (unsigned) i < errors.size; i ++) {
        printf("\n%d %s\n", errors.data[i].first, errors.data[i].second);
    }*/
    /********** testing ends **********/

    VectorStr_Clear(&sourceCode);
    VectorPairIntStr_Clear(&errors);
    MapStrToPairStrInt_Clear(&instructionSet);

    MapStrToInt_Clear(&labels);
    VectorListingCustom_Clear(&sourceTable);
    VectorInt_Clear(&programCounter);

    return 0;
}