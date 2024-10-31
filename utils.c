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
            map->data[i].value.first = strdup(first);
            map->data[i].value.second = second;
            return;
        }
    }

    if (map->size == map->capacity) {
        map->capacity *= 2;
        map->data = realloc(map->data, map->capacity * sizeof(_MapEntryStrToPairStrInt));
    }
    
    map->data[map->size].key = strdup(key);
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