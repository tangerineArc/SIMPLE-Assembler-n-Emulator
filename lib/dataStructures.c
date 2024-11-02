#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "../headers/dataStructures.h"
#include "../headers/utils.h"

/**************************************
    utility functions for VectorInt    
**************************************/

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

/**************************************
    utility functions for VectorStr    
**************************************/

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

/***************************************
    utility functions for PairIntStr    
***************************************/

/***** comapres two PairIntStr's a and b; return a->first - b->first *****/
int PairIntStr_Compare(const void* a, const void* b) {
    PairIntStr* pairA = (PairIntStr*) a;
    PairIntStr* pairB = (PairIntStr*) b;
    return pairA->first - pairB->first;
}

/*********************************************
    utility functions for VectorPairIntStr    
*********************************************/

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

/***** sorts the vector as per the first item in the stored pairs *****/
void VectorPairIntStr_Sort(VectorPairIntStr* vector) {
    qsort(vector->data, vector->size, sizeof(PairIntStr), PairIntStr_Compare);
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

/***********************************************
    utility functions for MapStrToPairStrInt    
***********************************************/

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

/****************************************
    utility functions for MapStrToInt    
****************************************/

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

/************************************************
    utility functions for VectorListingCustom    
************************************************/

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

    vector->capacity = newCapacity;
    vector->size = newCapacity;

    for (i = 0; i < vector->size; i ++) {
        vector->data[i].label = (char*) realloc(vector->data[i].label, 256);
        vector->data[i].mnemonic = (char*) realloc(vector->data[i].mnemonic, 256);
        vector->data[i].operand = (char*) realloc(vector->data[i].operand, 256);
    }
}

void VectorListingCustom_Insert(VectorListingCustom* vector, int index, Field field, const void* value) {
    ListingCustom* entry;

    if (index < 0 || index >= vector->capacity) {
        printf("INTERNAL_ERROR: index out of bounds\n");
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