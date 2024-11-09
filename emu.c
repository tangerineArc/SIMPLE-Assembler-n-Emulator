/*********************************************************************
------------------- DECLARATION OF AUTHORSHIP ------------------------
I hereby declare that the source-code presented herein is my own work.
    --- Author Name: SWAGATAM PATI
    --- Roll No.: 2301AI28
----------------------------------------------------------------------
*********************************************************************/

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "./headers/dataStructures.h"
#include "./headers/utils.h"

VectorInt machineCode; /* stores machine-code from assembled file */
MapStrToPairStrInt* instructionSet; /* stores {mnemonic : (opcode, numOperands)} */

int virtualMemory[1 << 24]; /* allocates memory for the emulator */
int read_write[2]; /* stores a pair of read-write locations */

int numInstructions = 0; /* total number instructions executed */
int A, B, PC, SP; /* register A, register B, program-counter, stack-pointer */

const char* mnemonics[] = {
    "ldc", "adc", "ldl", "stl", "ldnl", "stnl", "add", "sub", "shl", "shr", "adj", "a2sp", "sp2a", "call", "return", "brz", "brlz", "br", "HALT"
};

/*******************************************
    functions implmenting ISA operations    
*******************************************/
void ldc(int value) {
    B = A;
    A = value;
}
void adc(int value) {
    A += value;
}
void ldl(int offset) {
    B = A;
    A = virtualMemory[SP + offset];
    read_write[0] = SP + offset;
    read_write[1] = 0;
}
void stl(int offset) {
    read_write[0] = SP + offset;
    read_write[1] = virtualMemory[SP + offset];
    virtualMemory[SP + offset] = A;
    A = B;
}
void ldnl(int offset) {
    A = virtualMemory[A + offset];
    read_write[0] = SP + offset;
    read_write[1] = 0;
}
void stnl(int offset) {
    read_write[0] = SP + offset;
    read_write[1] = virtualMemory[SP + offset];
    virtualMemory[A + offset] = B;
}
void add(int value) {
    A += B;
    value *= 0; /* to prevent compilation warnings */
}
void sub(int value) {
    A = B - A;
    value *= 0; /* to prevent compilation warnings */
}
void shl(int value) {
    A = B << A;
    value *= 0; /* to prevent compilation warnings */
}
void shr(int value) {
    A = B >> A;
    value *= 0; /* to prevent compilation warnings */
}
void adj(int value) {
    SP += value;
}
void a2sp(int value) {
    SP = A;
    A = B;
    value *= 0; /* to prevent compilation warnings */
}
void sp2a(int value) {
    B = A;
    A = SP;
    value *= 0; /* to prevent compilation warnings */
}
void call(int offset) {
    B = A;
    A = PC;
    PC += offset;
}
void return_(int value) {
    PC = A;
    A = B;
    value *= 0; /* to prevent compilation warnings */
}
void brz(int offset) {
    if (A == 0) PC += offset;
}
void brlz(int offset) {
    if (A < 0) PC += offset;
}
void br(int offset) {
    PC += offset;
}
void HALT(int value) {
    value *= 0; /* to prevent compilation warnings */
    return;
}

void (*operations[])(int) = {ldc, adc, ldl, stl, ldnl, stnl, add, sub, shl, shr, adj, a2sp, sp2a, call, return_, brz, brlz, br, HALT};

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
void parseAssembledFile(char* binaryFilePath) {
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

/******************************************************
    prints memory contents before program execution    
******************************************************/
void bdump(void) {
    size_t i;
    printf(">>> memory dump before program execution\n\n");
    for (i = 0; i < machineCode.size; i += 4) {
        int j;
        printf("\t%s : ", decimalToHexVE(i));
        for (j = i; j < minOf2Ints(machineCode.size, i + 4); ++ j) printf("%s ", decimalToHexVE(machineCode.data[j]));
        printf("\n");
    }
    printf("\n");
}

/*****************************************************************
    prints memory contents after program execution is finished    
*****************************************************************/
void adump(void) {
    size_t i;
    printf(">>> memory dump after program execution\n\n");
    for (i = 0; i < machineCode.size; i += 4) {
        int j;
        printf("\t%s : ", decimalToHexVE(i));
        for (j = i; j < minOf2Ints(machineCode.size, i + 4); ++ j) printf("%s ", decimalToHexVE(virtualMemory[j]));
        printf("\n");
    }
    printf("\n");
}

/*********************************************************
    carry out instructions as per the specified option    
*********************************************************/
void runEmulator(char* flag) {
    A = 0;
    B = 0;
    PC = 0;
    SP = sizeof(virtualMemory) / sizeof(virtualMemory[0]) - 1;
    
    if (!strcmp(flag, "-bdump")) {
        bdump();
        return;
    }

    while ((size_t) PC < machineCode.size) {
        int old_pc = PC;

        int curr = machineCode.data[PC];
        int opCode = curr & 0xff;
        int value = (curr - opCode) >> 8;

        if (opCode > 18 || opCode < 0) {
            fprintf(stdout, "::: reached end of program without HALT\n");
            break;
        }

        operations[opCode](value);

        PC ++;
        
        if (PC < 0 || (size_t) PC > machineCode.size || numInstructions > (1 << 24)) {
            fprintf(stderr, "::: ERROR: segmentation fault: exceeded virtual-memory limit\n");
            break;
        }
        if (PC == old_pc) {
            fprintf(stderr, "::: ERROR: encountered infinite loop\n");
            break;
        }

        numInstructions ++;

        if (!strcmp(flag, "-trace")) {
            printf(">>> PC = %08x, SP = %08x, A = %08x, B = %08x ", PC, SP, A, B);
            printf("%s ", mnemonics[opCode]);
            if (MapStrToPairStrInt_Find(instructionSet, mnemonics[opCode])->second > 0) printf("%s", decimalToHexVE(value));
            printf("\n");
        } else if (!strcmp(flag, "-reads") && (opCode == 2 || opCode == 4)) {
            printf(">>> reading from location [%s] --- value [%s]\n", decimalToHexVE(read_write[0]), decimalToHexVE(A));
        } else if (!strcmp(flag, "-writes") && (opCode == 3 || opCode == 5)) {
            printf(">>> writing to location [%s] --- changed value from [%s] to [%s]\n", decimalToHexVE(read_write[0]), decimalToHexVE(read_write[1]), decimalToHexVE(virtualMemory[read_write[0]]));
        }
        
        if (opCode >= 18) break;
    }
    
    if (!strcmp(flag, "-trace")) printf("\n>>> program execution finished\n");
    else if (!strcmp(flag, "-adump")) adump();

    printf(">>> %d instructions executed\n\n", numInstructions);
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
        printf("\t-trace   show instruction trace\n");
        printf("\t-reads   show memory reads\n");
        printf("\t-writes  show memory writes\n");
        printf("\t-bdump   show memory dump before execution\n");
        printf("\t-adump   show memory dump after execution\n");
        printf("\t-isa     show instruction set\n");
        printf("\t-man     show emulator manual\n");
        printf("\n--------------------- EMULATOR MANUAL ENDS ---------------------\n\n");
        return 0;
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
        return 0;
    }

    if (!strcmp(argv[1], "-trace") || !strcmp(argv[1], "-reads") || !strcmp(argv[1], "-writes") || !strcmp(argv[1], "-bdump") || !strcmp(argv[1], "-adump")) {
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

    parseAssembledFile(argv[2]);

    initializeInstructionSet();

    runEmulator(argv[1]);

    MapStrToPairStrInt_Clear(instructionSet);
    VectorInt_Clear(&machineCode);

    return 0;
}
