#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "./headers/dataStructures.h"
#include "./headers/utils.h"

int virtualMemory[1 << 24];
int change[2];

int A, B, SP;
int idx, cont, exec;

VectorStr* machineCode;
VectorStr* mnemonics;

void readMachineCode(char* binaryFilePath) {
    FILE* binaryFile = fopen(binaryFilePath, "rb");

    unsigned int read;
    int pos = 0;

    machineCode = VectorStr_Initialize();
    
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
        VectorStr_Push(machineCode, decimalToHexVE(read));
    }
    
    if (fclose(binaryFile) != 0) {
        fprintf(stderr, "\n::: EMULATOR_ERROR: could not close file \"%s\"\n\n", binaryFilePath);
        exit(-4);
    }
}

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

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "\n::: EMULATOR_ERROR: usage: ./emulate.exe <executable-file-path>\n\n");
        exit(-1);
    }

    readMachineCode(argv[1]);

    initializeMnemonics();

    VectorStr_Clear(mnemonics);
    VectorStr_Clear(machineCode);

    return 0;
}