#include <stdbool.h>
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
        fprintf(stderr, "ASSEMBLER_ERROR: could not allocate memory\n");
        return NULL;
    }

    while ((ch = fgetc(filePtr)) != EOF && ch != '\n') {
        if (len + 1 >= capacity) {
            capacity *= 2;
            buffer = realloc(buffer, capacity);
            if (!buffer) {
                fprintf(stderr, "ASSEMBLER_ERROR: could not allocate memory\n");
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

/**********************************
    returns a slice of a string    
**********************************/
char* substr(const char* string, int start, int length) {
    char* substring = malloc(length + 1);

    strncpy(substring, string + start, length);
    substring[length] = '\0';

    return substring;
}

/******************************************
    returns the minimum of two integers    
******************************************/
int minOf2Ints(int a, int b) {
    if (a < b) return a;
    return b;
}

/********************************************
    data-structure for vector of integers    
********************************************/
typedef struct {
    int* data;
    int size;
    int capacity;
} VectorInt;

/***** allocates memory *****/
void VectorInt_Initialize(VectorInt* vector) {
    vector->size = 0;
    vector->capacity = 4;
    vector->data = (int*) malloc(sizeof(int) * vector->capacity);
}

/***** resizes the vector to a specified new capacity *****/
void VectorInt_Resize(VectorInt* vector, int newCapacity) {
    if (newCapacity < vector->size) {
        vector->size = newCapacity;
    }
    
    vector->data = (int*) realloc(vector->data, sizeof(int) * newCapacity);
    
    if (newCapacity > vector->capacity) {
        int i;
        for (i = vector->capacity; i < newCapacity; i ++) {
            vector->data[i] = 0;
        }
    }
    
    vector->capacity = newCapacity;
}

/***** inserts a value at a specific index *****/
void VectorInt_Insert(VectorInt* vector, int index, int value) {
    int i;

    if (index < 0 || index > vector->size) {
        printf("INTERNAL_ERROR: index out of bounds");
        return;
    }

    if (vector->size >= vector->capacity) {
        VectorInt_Resize(vector, vector->capacity * 2);
    }

    for (i = vector->size; i > index; i --) {
        vector->data[i] = vector->data[i - 1];
    }

    vector->data[index] = value;
    vector->size ++;
}

/***** frees allocated memory *****/
void VectorInt_Clear(VectorInt* vector) {
    free(vector->data);
    vector->size = 0;
    vector->capacity = 0;
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
    vector->data = (char**) malloc(sizeof(char*) * vector->capacity);
}

/***** pushes a string to the end *****/
void VectorStr_Push(VectorStr* vector, const char* string) {
    if (vector->size >= vector->capacity) {
        vector->capacity *= 2;
        vector->data = (char**) realloc(vector->data, sizeof(char*) * vector->capacity);
    }
    
    vector->data[vector->size] = (char*) malloc(strlen(string) + 1);
    vector->data[vector->size] = strdup(string);
    vector->size ++;
}

/***** resizes the vector to a specified new capacity *****/
void VectorStr_Resize(VectorStr* vector, int newCapacity) {
    if (newCapacity < vector->size) {
        int i;
        for (i = newCapacity; i < vector->size; i ++) {
            free(vector->data[i]);
        }
        vector->size = newCapacity;
    }

    vector->data = (char**) realloc(vector->data, sizeof(char*) * newCapacity);
    
    if (newCapacity > vector->capacity) {
        int i;
        for (i = vector->capacity; i < newCapacity; i ++) {
            vector->data[i] = NULL;
        }
    }

    vector->capacity = newCapacity;
}

/***** inserts a string at a specific index *****/
void VectorStr_Insert(VectorStr* vector, int index, const char* string) {
    int i;

    if (index < 0 || index > vector->size) {
        printf("INTERNAL_ERROR: index out of bounds\n");
        return;
    }

    if (vector->size >= vector->capacity) {
        VectorStr_Resize(vector, vector->capacity * 2);
    }

    for (i = vector->size; i > index; i --) {
        vector->data[i] = vector->data[i - 1];
    }

    vector->data[index] = strdup(string);
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

/**********************************************
    data-structure for integer-string pairs    
**********************************************/
typedef struct {
    int first;
    char* second;
} PairIntStr;

/**********************************************
    data-structure for string-integer pairs    
**********************************************/
typedef struct {
    char* first;
    int second;
} PairStrInt;

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

/****************************************************************
    data-structure for map of strings to string-integer pairs    
****************************************************************/
typedef struct {
    char* key;
    PairStrInt value;
} _MapEntryStrToPairStrInt;

typedef struct {
    _MapEntryStrToPairStrInt* data;
    size_t size;
    size_t capacity;
} MapStrToPairStrInt;

/***** allocates memory *****/
void MapStrToPairStrInt_Initialize(MapStrToPairStrInt* map) {
    map->size = 0;
    map->capacity = 4;
    map->data = malloc(sizeof(_MapEntryStrToPairStrInt) * map->capacity);
}

/***** adds a string-integer pair entry for a given string key to the map *****/
void MapStrToPairStrInt_Add(MapStrToPairStrInt* map, const char* key, const char* first, int second) {
    size_t i;
    for (i = 0; i < map->size; i ++) {
        if (strcmp(map->data[i].key, key) == 0) {
            map->data[i].value.first = (char*) malloc(strlen(first) + 1);
            map->data[i].value.first = strdup(first);

            map->data[i].value.second = second;

            return;
        }
    }

    if (map->size == map->capacity) {
        map->capacity *= 2;
        map->data = realloc(map->data, map->capacity * sizeof(_MapEntryStrToPairStrInt));
    }
    
    map->data[map->size].key = (char*) malloc(strlen(key) + 1);
    map->data[map->size].key = strdup(key);

    map->data[map->size].value.first = (char*) malloc(strlen(first) + 1);
    map->data[map->size].value.first = strdup(first);

    map->data[map->size].value.second = second;

    map->size ++;
}

/***** returns the string-integer pair corresponding to a given searchKey *****/
PairStrInt* MapStrToPairStrInt_Find(MapStrToPairStrInt* map, const char* key) {
    size_t i;
    for (i = 0; i < map->size; i ++) {
        if (strcmp(map->data[i].key, key) == 0) {
            return &map->data[i].value;
        }
    }
    return NULL;
}

/***** frees allocated memory *****/
void MapStrToPairStrInt_Clear(MapStrToPairStrInt* map) {
    size_t i;
    for (i = 0; i < map->size; i ++) {
        free(map->data[i].key);
        free(map->data[i].value.first);
    }
    free(map->data);
    map->data = NULL;
    map->size = 0;
    map->capacity = 0;
}

/****************************************************
    data-structure for map of strings to integers    
****************************************************/
typedef struct {
    char* key;
    int value;
} _MapEntryStrToInt;

typedef struct {
    _MapEntryStrToInt* data;
    size_t size;
    size_t capacity;
} MapStrToInt;

/***** allocates memory *****/
void MapStrToInt_Initialize(MapStrToInt* map) {
    map->size = 0;
    map->capacity = 4;
    map->data = malloc(sizeof(_MapEntryStrToInt) * map->capacity);
}

/***** adds an integer entry for a given string key to the map *****/
void MapStrToInt_Add(MapStrToInt* map, const char* key, int value) {
    size_t i;
    for (i = 0; i < map->size; i ++) {
        if (strcmp(map->data[i].key, key) == 0) {
            map->data[i].value = value;
            return;
        }
    }

    if (map->size == map->capacity) {
        map->capacity *= 2;
        map->data = realloc(map->data, map->capacity * sizeof(_MapEntryStrToInt));
    }
    
    map->data[map->size].key = strdup(key);
    map->data[map->size].value = value;
    map->size ++;
}

/***** returns a pointer to the integer corresponding to a given searchKey *****/
int* MapStrToInt_Find(MapStrToInt* map, const char* key) {
    size_t i;
    for (i = 0; i < map->size; i ++) {
        if (strcmp(map->data[i].key, key) == 0) {
            return &map->data[i].value;
        }
    }
    return NULL;
}

/***** frees allocated memory *****/
void MapStrToInt_Clear(MapStrToInt* map) {
    size_t i;
    for (i = 0; i < map->size; i ++) {
        free(map->data[i].key);
    }
    free(map->data);
    map->data = NULL;
    map->size = 0;
    map->capacity = 0;
}

/********************************************************************
    data-structure for listing of 3 strings, 1 integer, 1 boolean    
********************************************************************/
typedef struct {
    char* label;
    char* mnemonic;
    char* operand;
    int operandType;
    bool isLabelPresent;
} ListingCustom;

/***************************************************
    data-structure for vector of custom listings    
***************************************************/
typedef struct {
    ListingCustom* data;
    int size;
    int capacity;
} VectorListingCustom;

/***** allocates memory *****/
void VectorListingCustom_Initialize(VectorListingCustom* vector) {
    int i;

    vector->size = 0;
    vector->capacity = 4;
    vector->data = (ListingCustom*) malloc(sizeof(ListingCustom) * vector->capacity);

    for (i = 0; i < vector->capacity; i ++) {
        vector->data[i].label = (char*) malloc(256);
        vector->data[i].mnemonic = (char*) malloc(256);
        vector->data[i].operand = (char*) malloc(256);
    }
}

/***** resizes the vector to a specified new capacity *****/
void VectorListingCustom_Resize(VectorListingCustom* vector, int newCapacity) {
    int i;

    vector->data = (ListingCustom*) realloc(vector->data, sizeof(ListingCustom) * newCapacity);

    for (i = vector->capacity; i < newCapacity; i ++) {
        vector->data[i].label = (char*) malloc(256);
        vector->data[i].mnemonic = (char*) malloc(256);
        vector->data[i].operand = (char*) malloc(256);
    }

    vector->capacity = newCapacity;
    vector->size = newCapacity;
}

/***** inserts a specific field at a specified index *****/
typedef enum {
    LABEL,
    MNEMONIC,
    OPERAND,
    OPERAND_TYPE,
    IS_LABEL_PRESENT
} Field;

void VectorListingCustom_Insert(VectorListingCustom* vector, int index, Field field, const void* value) {
    ListingCustom* entry;

    if (index < 0 || index >= vector->capacity) {
        printf("Index out of bounds\n");
        return;
    }
    
    entry = &vector->data[index];
    
    switch (field) {
        case LABEL:
            entry->label = (char*) realloc(entry->label, strlen((char*) value) + 1);
            entry->label = strdup((const char*) value);
            break;

        case MNEMONIC:
            entry->mnemonic = (char*) realloc(entry->mnemonic, strlen((char*) value) + 1);
            entry->mnemonic = strdup((const char*) value);
            break;

        case OPERAND:
            entry->operand = (char*) realloc(entry->operand, strlen((char*) value) + 1);
            entry->operand = strdup((const char*) value);
            break;

        case OPERAND_TYPE:
            entry->operandType = *(const int*) value;
            break;

        case IS_LABEL_PRESENT:
            entry->isLabelPresent = *(const bool*) value;
            break;
    }
}

/***** frees allocated memory *****/
void VectorListingCustom_Clear(VectorListingCustom* vector) {
    int i;
    for (i = 0; i < vector->size; i ++) {
        free(vector->data[i].label);
        free(vector->data[i].mnemonic);
        free(vector->data[i].operand);
    }
    free(vector->data);
    vector->data = NULL;
    vector->size = 0;
    vector->capacity = 0;
}