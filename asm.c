#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "./utils.c"

typedef struct {
    char** data;
    int size;
    int capacity;
} Vector;

void initVector(Vector *vec) {
    vec->size = 0;
    vec->capacity = 4;
    vec->data = malloc(sizeof(char*) *vec->capacity);
}
void addElement(Vector* vec, const char* str) {
    if (vec->size >= vec->capacity) {
        vec->capacity *= 2;
        vec->data = realloc(vec->data, sizeof(char*) *vec->capacity);
    }
    vec->data[vec->size] = strdup(str);
    vec->size++;
}
void freeVector(Vector* vec) {
    int i;
    for (i = 0; i < vec->size; i++) {
        free(vec->data[i]);
    }
    free(vec->data);
}

const char* getElement(Vector* vec, int index) {
    if (index >= 0 && index < vec->size) {
        return vec->data[index];
    }
    return NULL;
}

/***************************************
    formats source code line-by-line    
***************************************/
char* trim(char* line, int size) {
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
        printf("Invalid Syntax.\n");
    }
    return trimmedStr;
}

Vector sourceCode;

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

    initVector(&sourceCode);
    
    while ((line = readLine(sourceFile)) != NULL) {
        char* tmp = trim(line, strlen(line));
        if (strlen(tmp) > 0) {
            addElement(&sourceCode, tmp);
        }
    }

    fclose(sourceFile);
}

int main(int argc, char* argv[]) {
    int i;

    if (argc != 2) {
        fprintf(stderr, "ASSEMBLER_ERROR: Usage: ./a <source-file-path>\n");
        exit(-1);
    }

    executePass1(argv[1]);

    printf("%d\n", sourceCode.size);
    for (i = 0; i < sourceCode.size; i ++) {
        printf("%s\n", getElement(&sourceCode, i));
    }

    freeVector(&sourceCode);

    return 0;
}