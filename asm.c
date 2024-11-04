#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "./headers/dataStructures.h"
#include "./headers/utils.h"

VectorStr* sourceCode; /* stores trimmed and formatted source-code */ /* DONE */
VectorPairIntStr errors; /* stores errors as pairs of lineNumber & message */ /* DONE */
MapStrToPairStrInt* instructionSet; /* stores {mnemonic : (opcode, numOperands)} */ /* DONE */
MapStrToInt* labels; /* stores {label : line of declaration} */ /* DONE */
VectorListingCustom sourceTable; /* stores parts of source-code lines depending on category */ /* DONE */
VectorInt programCounter; /* maintains the PC */ /* DONE */
VectorPairIntStr machineCode; /* stores the machine-code */

bool isHaltPresent = false;

/******************************
    generate error messages    
******************************/
void raiseError(int lineNumber, const char* message) {
    char errorMessage[256];
    sprintf(errorMessage, "ERROR on line %d: %s", lineNumber, message);

    VectorPairIntStr_Push(&errors, lineNumber + 1, errorMessage);
}

/***************************************
    formats source code line-by-line    
***************************************/
char* trim(const char* line, int lineNumber) {
    size_t length = strlen(line);
    char* trimmedStr = (char*) malloc(length + 1);

    char* start = (char*) line;
    char* end = (char*) line + length - 1;

    const char* p = NULL;

    size_t idx = 0;

    size_t i;

    int spaces = 0;

    if (!trimmedStr) {
        fprintf(stderr, "::: INTERNAL_ERROR: memory allocation failed in \"char* trim(char* line, int lineNumber)\"");
        exit(-4);
    }

    while (end > start && (isspace((unsigned char) *end))) {
        end --;
    }
    end ++;

    while (start < end && (isspace((unsigned char) *start))) {
        start ++;
    }

    for (p = start; p < end; p ++) {
        if (*p == ';') {
            break;
        }

        if (*p == ':') {
            trimmedStr[idx ++] = ':';
            if (p + 1 < end && !isspace((unsigned char) *(p + 1))) {
                trimmedStr[idx ++] = ' ';
            }
            continue;
        }

        if (!isspace((unsigned char) *p)) {
            trimmedStr[idx ++] = *p;
            continue;
        }

        trimmedStr[idx ++] = ' ';
        while (p + 1 < end && isspace((unsigned char) *(p + 1))) {
            p ++;
        }
    }

    while (idx > 0 && isspace((unsigned char) trimmedStr[idx - 1])) {
        idx --;
    }

    trimmedStr[idx] = '\0';

    for (i = 0; i < idx; i ++) {
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
    generates a map of {mnemonic : (opcode, numOperands)}    
************************************************************/
void initializeInstructionSet(void) {
    instructionSet = MapStrToPairStrInt_Initialize();

    MapStrToPairStrInt_Add(instructionSet, "data",     "", 1);
    MapStrToPairStrInt_Add(instructionSet, "ldc",    "00", 1);
    MapStrToPairStrInt_Add(instructionSet, "adc",    "01", 1);
    MapStrToPairStrInt_Add(instructionSet, "ldl",    "02", 2);
    MapStrToPairStrInt_Add(instructionSet, "stl",    "03", 2);
    MapStrToPairStrInt_Add(instructionSet, "ldnl",   "04", 2);
    MapStrToPairStrInt_Add(instructionSet, "stnl",   "05", 2);
    MapStrToPairStrInt_Add(instructionSet, "add",    "06", 0);
    MapStrToPairStrInt_Add(instructionSet, "sub",    "07", 0);
    MapStrToPairStrInt_Add(instructionSet, "shl",    "08", 0);
    MapStrToPairStrInt_Add(instructionSet, "shr",    "09", 0);
    MapStrToPairStrInt_Add(instructionSet, "adj",    "0A", 1);
    MapStrToPairStrInt_Add(instructionSet, "a2sp",   "0B", 0);
    MapStrToPairStrInt_Add(instructionSet, "sp2a",   "0C", 0);
    MapStrToPairStrInt_Add(instructionSet, "call",   "0D", 2);
    MapStrToPairStrInt_Add(instructionSet, "return", "0E", 0);
    MapStrToPairStrInt_Add(instructionSet, "brz",    "0F", 2);
    MapStrToPairStrInt_Add(instructionSet, "brlz",   "10", 2);
    MapStrToPairStrInt_Add(instructionSet, "br",     "11", 2);
    MapStrToPairStrInt_Add(instructionSet, "HALT",   "12", 0);
    MapStrToPairStrInt_Add(instructionSet, "SET",      "", 1);
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
void parseLabels(void) {
    int i;

    labels = MapStrToInt_Initialize();

    for (i = 0; i < sourceCode->size; i ++) {
        const char* currentLine = sourceCode->data[i];

        char label[100] = {0};
        size_t idx = 0;

        unsigned int j;
        for (j = 0; currentLine[j] != '\0'; j ++) {
            if (currentLine[j] == ':') {
                int* labelPos;

                label[idx] = '\0';

                if (!checkLabelIdentifierValidity(label)) {
                    raiseError(i + 1, "invalid label identifier");
                    break;
                }

                labelPos = MapStrToInt_Find(labels, label);
                if (labelPos != NULL) {
                    char* errorMessage = (char*) malloc(strlen("multiple declarations found for label: ") + strlen(label) + 1);

                    if (strlen(currentLine) > j + 4 && strncmp(currentLine + j + 2, "SET", 3) == 0) {
                        continue;
                    }
                    
                    if (strlen(currentLine) > j + 5 && strncmp(currentLine + j + 2, "data", 4) == 0 && *labelPos < 0) {
                        *labelPos = i;
                        free(labelPos);
                        continue;
                    }
                    
                    sprintf(errorMessage, "multiple declarations found for label: %s", label);
                    raiseError(i + 1, errorMessage);
                    free(errorMessage);
                } else {
                    int labelValue = (j + 4 < strlen(currentLine) && strncmp(currentLine + j + 2, "SET", 3) == 0) ? -i : i;
                    MapStrToInt_Add(labels, label, labelValue);
                }
                break;
            }
            label[idx ++] = currentLine[j];
        }
    }
}

/***********************************************************************************************
    modifies sourceCode vector by translating pseudo-instructions (SET) to real-instructions    
***********************************************************************************************/
void translatePseudoInstructions(void) {
    VectorStr* realInstructionsSourceCode = VectorStr_Initialize();

    int i;
    for (i = 0; i < sourceCode->size; i ++) {
        char* token = (char*) malloc(strlen(sourceCode->data[i]) + 1);
        bool state = false;

        long unsigned int j;
        token[0] = '\0';
        for (j = 0; j < strlen(sourceCode->data[i]); j ++) {
            strncat(token, &sourceCode->data[i][j], 1);
            
            if (sourceCode->data[i][j] == ':') {
                if (strlen(token) > 0) {
                    token[strlen(token) - 1] = '\0';
                }

                if (strlen(sourceCode->data[i]) > j + 5 && !strcmp(substr(sourceCode->data[i], j + 2, 3), "SET")) {
                    state = true;

                    if (abs(*MapStrToInt_Find(labels, token)) == i) {
                        char* info = (char*) malloc(256);

                        MapStrToInt_Add(labels, token, realInstructionsSourceCode->size - 1);

                        sprintf(info, "%s data %s", substr(sourceCode->data[i], 0, j + 1), substr(sourceCode->data[i], j + 6, strlen(sourceCode->data[i]) - (j + 6)));
                        VectorStr_Push(realInstructionsSourceCode, info);
                        free(info);
                    } else {
                        char* _t1 = (char*) malloc(256);
                        char* _t2 = (char*) malloc(256);

                        if (strlen(sourceCode->data[i]) <= j + 5) {
                            free(_t1);
                            free(_t2);
                            break;
                        }

                        VectorStr_Push(realInstructionsSourceCode, "adj 10000");
                        VectorStr_Push(realInstructionsSourceCode, "stl -1");
                        VectorStr_Push(realInstructionsSourceCode, "stl 0");
                        
                        sprintf(_t1, "ldc %s", substr(sourceCode->data[i], j + 6, strlen(sourceCode->data[i]) - (j + 6)));
                        VectorStr_Push(realInstructionsSourceCode, _t1);
                        free(_t1);

                        sprintf(_t2, "ldc %s", substr(token, 0, j));
                        VectorStr_Push(realInstructionsSourceCode, _t2);
                        free(_t2);

                        VectorStr_Push(realInstructionsSourceCode, "stnl 0");
                        VectorStr_Push(realInstructionsSourceCode, "ldl 0");
                        VectorStr_Push(realInstructionsSourceCode, "ldl -1");
                        VectorStr_Push(realInstructionsSourceCode, "adj -1000");
                    }
                    break;
                }
            }
        }

        if (!state && strlen(sourceCode->data[i]) != 0) {
            VectorStr_Push(realInstructionsSourceCode, sourceCode->data[i]);
        }

        free(token);
    }

    sourceCode = realInstructionsSourceCode;
}

/**************************************************************
    separates data segments from instructions in sourceCode    
**************************************************************/
void separateDataFromInstructions(void) {
    VectorStr* properlySeparatedSoureCode = VectorStr_Initialize();
    VectorStr* dataSegments = VectorStr_Initialize();

    int i;
    for (i = 0; i < sourceCode->size; i ++) {
        bool state = false;

        unsigned int length = strlen(sourceCode->data[i]);

        unsigned int j;
        for (j = 0; j < length; j ++) {
            if (!strcmp(substr(sourceCode->data[i], j, 4), "data") && j + 4 < length) {
                VectorStr_Push(dataSegments, sourceCode->data[i]);
                state = true;
                break;
            }
            if (sourceCode->data[i][length - 1] == ':' && i + 1 < sourceCode->size && !strcmp(substr(sourceCode->data[i + 1], 0, 4), "data")) {
                VectorStr_Push(dataSegments, sourceCode->data[i]);
                state = true;
                break;
            }
        }
        if (!state) {
            VectorStr_Push(properlySeparatedSoureCode, sourceCode->data[i]);
        }
    }

    for (i = 0; i < dataSegments->size; i ++) {
        VectorStr_Push(properlySeparatedSoureCode, dataSegments->data[i]);
    }

    sourceCode = properlySeparatedSoureCode;
    VectorStr_Clear(dataSegments);
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
void tabulateSourceCode(void) {
    int PC = 0;

    int i;
    for (i = 0; i < sourceCode->size; i ++) {
        VectorStr* row = VectorStr_Initialize();

        char* curr = (char*) malloc(256);

        unsigned int length = strlen(sourceCode->data[i]);

        int ptr = 1;

        unsigned int j;

        int opType;

        curr[0] = '\0';

        VectorStr_Resize(row, 10);

        for (j = 0; j < length; j ++) {
            if (sourceCode->data[i][j] == ':') {
                VectorStr_Insert(row, 0, curr);
                curr[0] = '\0';

                j ++;
                continue;
            } else if (sourceCode->data[i][j] == ' ') {
                VectorStr_Insert(row, ptr ++, curr);
                curr[0] = '\0';

                continue;
            }

            strncat(curr, &sourceCode->data[i][j], 1);
            if (j == length - 1) {
                VectorStr_Insert(row, ptr ++, curr);
            }
        }
        
        if (strlen(row->data[1]) != 0) {
            bool tmp = true;
            VectorListingCustom_Insert(&sourceTable, &tmp, i, "isLabelPresent");
        } else {
            bool tmp = false;
            VectorListingCustom_Insert(&sourceTable, &tmp, i, "isLabelPresent");
        }
        
        if (!strcmp(row->data[1], "HALT")) {
            isHaltPresent = true;
        }
        
        if (strlen(row->data[0]) != 0) {
            MapStrToInt_Add(labels, row->data[0], PC);
        }
        
        VectorInt_Insert(&programCounter, i, PC);
    
        if (ptr == 1) {
            int val = 0;
            VectorListingCustom_Insert(&sourceTable, row->data[0], i, "label");
            VectorListingCustom_Insert(&sourceTable, "", i, "mnemonic");
            VectorListingCustom_Insert(&sourceTable, "", i, "operand");
            VectorListingCustom_Insert(&sourceTable, &val, i, "operandType");
            VectorStr_Clear(row);
            free(curr);
            continue;
        }
        
        PC ++;
        
        if (MapStrToPairStrInt_Find(instructionSet, row->data[1]) == NULL) {
            raiseError(i + 1, "invalid mnemonic");

            VectorStr_Clear(row);
            free(curr);
            continue;
        }
        
        if (minOf2Ints(MapStrToPairStrInt_Find(instructionSet, row->data[1])->second, 1) != ptr - 2) {
            raiseError(i + 1, "invalid OPCode-syntax combination");

            VectorStr_Clear(row);
            free(curr);
            continue;
        }
        
        VectorListingCustom_Insert(&sourceTable, row->data[0], i, "label");
        VectorListingCustom_Insert(&sourceTable, row->data[1], i, "mnemonic");
        VectorListingCustom_Insert(&sourceTable, row->data[2], i, "operand");
        opType = getOperandType(row->data[2]);
        VectorListingCustom_Insert(&sourceTable, &opType, i, "operandType");

        if (sourceTable.data[i].operandType == 1 && MapStrToInt_Find(labels, sourceTable.data[i].operand) == NULL) {
            raiseError(i + 1, "no such label");
        } else if (sourceTable.data[i].operandType == -1) {
            raiseError(i + 1, "invalid number");
        }

        free(curr);
        VectorStr_Clear(row);
    }
}

/******************************************************
    executes the first pass of the assembly process    
******************************************************/
void executePass1(char* sourceFilePath) {
    FILE* sourceFile = fopen(sourceFilePath, "r");

    char* line = NULL;

    if (sourceFile == NULL) {
        fprintf(stderr, "\n::: ASSEMBLER_ERROR: could not open file \"%s\"\n\n", sourceFilePath);
        exit(-2);
    }
    
    printf("\n>>> loaded \"%s\"\n", sourceFilePath);

    sourceCode = VectorStr_Initialize();
    
    VectorPairIntStr_Initialize(&errors);
    
    while ((line = readLine(sourceFile)) != NULL) {
        char* trimmedLine = trim(line, sourceCode->size);
        VectorStr_Push(sourceCode, trimmedLine);
        
        free(line);
        free(trimmedLine);
    }

    if (fclose(sourceFile) != 0) {
        fprintf(stderr, "\n::: ASSEMBLER_ERROR: could not close file \"%s\"\n\n", sourceFilePath);
        exit(-3);
    } else {
        printf(">>> parsed \"%s\"\n\n", sourceFilePath);
    }

    initializeInstructionSet();
    
    parseLabels();

    if (errors.size == 0) {
        translatePseudoInstructions();
    }

    VectorListingCustom_Initialize(&sourceTable);
    VectorListingCustom_Resize(&sourceTable, sourceCode->size);

    VectorInt_Initialize(&programCounter);
    VectorInt_Resize(&programCounter, sourceCode->size);
    
    separateDataFromInstructions();
    
    tabulateSourceCode();
}

/**************************************************************************
    output the errors encountered; returns false if no errors else true    
**************************************************************************/
bool logAssemblyProcess(char* sourceFilePath) {
    int i;

    printf("\n-------------------- ASSEMBLER LOG STARTS --------------------\n\n");
    
    if (errors.size == 0) {
        printf(">>> SUCCESS: assembly completed for \"%s\"\n", sourceFilePath);
        if (!isHaltPresent) {
            printf(">>> WARNING: HALT not present\n");
        }
        printf(">>> 0 errors encountered\n");
        printf(">>> generated binary file \"machine-code.out\" in current directory\n");
        printf(">>> generated listing file \"list-code.list\" in current directory\n");
        
        printf("\n--------------------- ASSEMBLER LOG ENDS ---------------------\n");

        return false;
    }

    printf(">>> %ld errors encountered\n", errors.size);

    VectorPairIntStr_Sort(&errors);
    for (i = 0; (unsigned) i < errors.size; i ++) {
        printf("\t::: %s\n", errors.data[i].second);
    }

    printf("\n--------------------- ASSEMBLER LOG ENDS ---------------------\n");
    
    return true;
}

int main(int argc, char* argv[]) {
    /********** for testing **********/
    unsigned int i;
    /********** testing ends **********/

    if (argc != 2) {
        fprintf(stderr, "\n::: ASSEMBLER_ERROR: Usage: ./assemble.exe <source-file-path>\n\n");
        exit(-1);
    }

    executePass1(argv[1]);
    
    /********** for testing **********/
    /* for (i = 0; i < (unsigned) sourceCode->size; i ++) {
        printf("%s\n", sourceCode->data[i]);
    }
    printf("\n");
    for (i = 0; i < errors.size; i ++) {
        printf("%d %s\n", errors.data[i].first, errors.data[i].second);
    }
    printf("\n");
    for (i = 0; i < labels->size; i ++) {
        printf("%s %d\n", labels->data[i].key, labels->data[i].value);
    }
    printf("\n");
    for (i = 0; i < sourceTable.size; i ++) {
        printf("%s %s %s %d %d\n", sourceTable.data[i].label, sourceTable.data[i].mnemonic, sourceTable.data[i].operand, sourceTable.data[i].operandType, sourceTable.data[i].isLabelPresent);
    } */
    /********** testing ends **********/

    if (!logAssemblyProcess(argv[1])) {
        /* executePass2(); */
        printf("yay\n");
    } else {
        printf("oh no\n");
    }

    VectorStr_Clear(sourceCode);
    VectorPairIntStr_Clear(&errors);
    MapStrToPairStrInt_Clear(instructionSet);
    MapStrToInt_Clear(labels);
    VectorListingCustom_Clear(&sourceTable);
    VectorInt_Clear(&programCounter);
    /* VectorPairIntStr_Clear(&machineCode); */

    return 0;
}