#include <stdio.h>
#include <stdlib.h>

/****************************************************
    executes the first pass of the assembly cycle    
****************************************************/
void executePass1(char* sourceFile) {
    FILE *fInPtr = fopen(sourceFile, "r");

    if (fInPtr == NULL) {
        printf("Error opening file.\n");
        exit(-2);
    } else {
        printf("File loaded successfully.\n");
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: ./a <source-file-path>");
        exit(-1);
    }

    executePass1(argv[1]);

    return 0;
}