#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "./headers/dataStructures.h"
#include "./headers/utils.h"

MapStrToPairStrInt* instructionSet; /* stores {mnemonic : (opcode, numOperands)} */
VectorInt machineCode; /* stores machine-code from assembled file */

int virtualMemory[1 << 24]; /* allocates memory for the emulator */

VectorStr* mnemonics;

/***********************************************
    generates an array of accepted mnemonics    
***********************************************/
void initializeMnemonics(void) {
    mnemonics = VectorStr_Initialize();

    VectorStr_Push(mnemonics, "ldc");
    VectorStr_Push(mnemonics, "adc");
    VectorStr_Push(mnemonics, "ldl");
    VectorStr_Push(mnemonics, "stl");
    VectorStr_Push(mnemonics, "ldnl");
    VectorStr_Push(mnemonics, "stnl");
    VectorStr_Push(mnemonics, "add");
    VectorStr_Push(mnemonics, "sub");
    VectorStr_Push(mnemonics, "shl");
    VectorStr_Push(mnemonics, "shr");
    VectorStr_Push(mnemonics, "adj");
    VectorStr_Push(mnemonics, "a2sp");
    VectorStr_Push(mnemonics, "sp2a");
    VectorStr_Push(mnemonics, "call");
    VectorStr_Push(mnemonics, "return");
    VectorStr_Push(mnemonics, "brz");
    VectorStr_Push(mnemonics, "brlz");
    VectorStr_Push(mnemonics, "br");
    VectorStr_Push(mnemonics, "HALT");
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

/*******************************************************************
    parses machine-code from assembled file and stores in memory    
*******************************************************************/
void readMachineCode(char* binaryFilePath) {
    FILE* binaryFile = fopen(binaryFilePath, "rb");

    unsigned int read;
    int pos = 0;

    VectorInt_Initialize(&machineCode);
    
    if (binaryFile == NULL) {
        fprintf(stderr, "\n::: EMULATOR_ERROR: could not open file \"%s\"\n\n", binaryFilePath);
        exit(-2);
    } else printf("\n>>> loaded \"%s\"\n", binaryFilePath);

    while (fread(&read, sizeof(unsigned int), 1, binaryFile) == 1) {
        if (pos >= (1 << 24)) {
            fprintf(stderr, "\n::: EMULATOR_ERROR: memory overflow\n\n");
            fclose(binaryFile);
            exit(-3);
        }

        virtualMemory[pos ++] = read;
        VectorInt_Insert(&machineCode, machineCode.size, read);
    }
    
    if (fclose(binaryFile) != 0) {
        fprintf(stderr, "\n::: EMULATOR_ERROR: could not close file \"%s\"\n\n", binaryFilePath);
        exit(-4);
    }
}

int main(int argc, char* argv[]) {
    if (argc != 3 && argc != 2) {
        fprintf(stderr, "\n::: EMULATOR_ERROR: usage: ./emulate.exe [OPTION] <executable-file-path>\n");
        printf("::: to see available options: ./emulate.exe -man\n");
        printf("::: to see instruction set: ./emulate.exe -isa\n\n");
        exit(-1);
    }

    if (strcmp(argv[1], "-man") == 0) {
        if (argc == 3) printf("\n>>> ignored everything specified after -man\n");
        printf("\n-------------------- EMULATOR MANUAL STARTS --------------------\n\n");
        printf(">>> usage: ./emulate.exe [OPTION] <executable-file-path>\n");
        printf(">>> available options:\n");
        printf("\t-trace  show instruction trace\n");
        printf("\t-read   show memory reads\n");
        printf("\t-write  show memory writes\n");
        printf("\t-bdump  show memory dump before execution\n");
        printf("\t-adump  show memory dump after execution\n");
        printf("\t-wipe   wipe written flags before execution\n");
        printf("\t-isa    show instruction set\n");
        printf("\t-man    show emulator manual\n");
        printf("\n--------------------- EMULATOR MANUAL ENDS ---------------------\n\n");
        exit(-1);
    } else if (strcmp(argv[1], "-isa") == 0) {
        if (argc == 3) printf("\n>>> ignored everything specified after -isa\n");
        printf("\n +--------+----------+---------+\n");
        printf(" | OPCODE | MNEMONIC | OPERAND |\n");
        printf(" +--------+----------+---------+\n");
        printf(" | 0      | ldc      | value   |\n");
        printf(" | 1      | adc      | value   |\n");
        printf(" | 2      | ldl      | value   |\n");
        printf(" | 3      | stl      | value   |\n");
        printf(" | 4      | ldnl     | value   |\n");
        printf(" | 5      | stnl     | value   |\n");
        printf(" | 6      | add      | -----   |\n");
        printf(" | 7      | sub      | -----   |\n");
        printf(" | 9      | shr      | -----   |\n");
        printf(" | 10     | adj      | value   |\n");
        printf(" | 11     | a2sp     | -----   |\n");
        printf(" | 12     | sp2a     | -----   |\n");
        printf(" | 13     | call     | offset  |\n");
        printf(" | 14     | return   | -----   |\n");
        printf(" | 15     | brz      | offset  |\n");
        printf(" | 16     | brlz     | offset  |\n");
        printf(" | 17     | br       | offset  |\n");
        printf(" | 18     | HALT     | -----   |\n");
        printf(" | --     | SET      | value   |\n");
        printf(" +--------+----------+---------+\n\n");
        exit(-1);
    }

    if (!strcmp(argv[1], "-trace") || !strcmp(argv[1], "-read") || !strcmp(argv[1], "-write") || !strcmp(argv[1], "-bdump") || !strcmp(argv[1], "-adump") || !strcmp(argv[1], "-wipe")) {
        if (argc == 2) {
            fprintf(stderr, "\n::: EMULATOR_ERROR: expected a file-path after \"%s\"\n\n", argv[1]);
            exit(-1);
        }
    } else {
        fprintf(stderr, "\n::: EMULATOR_ERROR: could not recognize flag \"%s\"\n", argv[1]);
        printf("::: to see available options: ./emulate.exe -man\n");
        printf("::: to see instruction set: ./emulate.exe -isa\n\n");
        exit(-1);
    }

    readMachineCode(argv[2]);

    initializeInstructionSet();
    initializeMnemonics();

    MapStrToPairStrInt_Clear(instructionSet);
    VectorInt_Clear(&machineCode);
    VectorStr_Clear(mnemonics);

    return 0;
}