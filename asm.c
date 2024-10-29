#include <stdio.h>
#include <stdlib.h>

#include "./utils.c"

/****************************************************
    executes the first pass of the assembly cycle    
****************************************************/
void executePass1(char* sourceFilePath) {
    FILE* sourceFile = fopen(sourceFilePath, "r");

    char* line = NULL;

    if (sourceFile == NULL) {
        fprintf(stderr, "ASSEMBLER_ERROR: Could not open file \"%s\"\n", sourceFilePath);
        exit(-2);
    } else {
        printf("Sucessfully loaded \"%s\"\n", sourceFilePath);
    }

    while ((line = readLine(sourceFile)) != NULL) {
        printf("%s\n", line);
        free(line);
    }

    fclose(sourceFile);
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "ASSEMBLER_ERROR: Usage: ./a <source-file-path>\n");
        exit(-1);
    }

    executePass1(argv[1]);

    return 0;
}