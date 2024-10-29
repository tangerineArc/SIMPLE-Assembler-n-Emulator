#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**********************************************
    reads a line from a file and returns it    
**********************************************/
char* readLine(FILE* filePtr) {
    int ch;
    size_t len = 0;
    size_t capacity = 128;

    char* buffer = malloc(capacity);

    if (!buffer) {
        fprintf(stderr, "ASSEMBLER_ERROR: Memory allocation error.\n");
        return NULL;
    }

    while ((ch = fgetc(filePtr)) != EOF && ch != '\n') {
        if (len + 1 >= capacity) {
            capacity *= 2;
            buffer = realloc(buffer, capacity);
            if (!buffer) {
                fprintf(stderr, "ASSEMBLER_ERROR: Memory allocation error.\n");
                return NULL;
            }
        }
        buffer[len++] = ch;
    }

    if (ch == EOF && len == 0) {
        free(buffer);
        return NULL;
    }

    buffer[len] = '\0';
    return buffer;
}


/*****************************************
    duplicates a string and returns it    
*****************************************/
char *strdup(const char *str) {
    size_t size = strlen(str) + 1;
    char* p = malloc(size);
    if (p) {
        memcpy(p, str, size);
    }
    return p;
}

/*********************************
    reverses a string in-place    
*********************************/
void strrev(char* str) {
    int i = 0;
    int j = strlen(str) - 1;

    if (!str) {
        return;
    }
    
    while (i < j) {
        char temp = str[i];
        str[i] = str[j];
        str[j] = temp;

        i ++;
        j --;
    }
}

/*******************************************
    data-structure for vector of strings    
*******************************************/
typedef struct {
    char** data;
    int size;
    int capacity;
} VectorStr;

/***** allocates memory *****/
void VectorStr_Initialize(VectorStr* vector) {
    vector->size = 0;
    vector->capacity = 4;
    vector->data = malloc(sizeof(char*) * vector->capacity);
}

/***** pushes a string to the end *****/
void VectorStr_Push(VectorStr* vector, const char* string) {
    if (vector->size >= vector->capacity) {
        vector->capacity *= 2;
        vector->data = realloc(vector->data, sizeof(char*) * vector->capacity);
    }
    vector->data[vector->size] = strdup(string);
    vector->size ++;
}

/***** frees allocated memory *****/
void VectorStr_Clear(VectorStr* vector) {
    int i;
    for (i = 0; i < vector->size; i ++) {
        free(vector->data[i]);
    }
    free(vector->data);
}

/***** get string at given index *****/
const char* VectorStr_Get(VectorStr* vector, int index) {
    if (index >= 0 && index < vector->size) {
        return vector->data[index];
    }
    return NULL;
}

/**********************************************
    data-structure for integer-string pairs    
**********************************************/
typedef struct {
    int first;
    char* second;
} PairIntStr;

/********************************************************
    data-structure for vector of integer-string pairs    
********************************************************/
typedef struct {
    size_t size;
    size_t capacity;
    PairIntStr* data;
} VectorPairIntStr;

/***** allocates memory *****/
void VectorPairIntStr_Initialize(VectorPairIntStr* vector) {
    vector->size = 0;
    vector->capacity = 4;
    vector->data = malloc(sizeof(PairIntStr) * vector->capacity);
}

/***** pushes an integer-string pair to the end *****/
void VectorPairIntStr_Push(VectorPairIntStr* vector, int first, const char* second) {
    if (vector->size == vector->capacity) {
        vector->capacity *= 2;
        vector->data = realloc(vector->data, vector->capacity * sizeof(PairIntStr));
    }
    
    vector->data[vector->size].first = first;
    vector->data[vector->size].second = malloc(strlen(second) + 1);
    strcpy(vector->data[vector->size].second, second);
    vector->size ++;
}

/***** frees allocated memory *****/
void VectorPairIntStr_Clear(VectorPairIntStr* vector) {
    size_t i;
    for (i = 0; i < vector->size; i ++) {
        free(vector->data[i].second);
    }
    free(vector->data);

    vector->data = NULL;
    vector->size = 0;
    vector->capacity = 0;
}