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
    size_t initialCapacity = 4;

    vector->data = (int*) malloc(sizeof(int) * initialCapacity);
    vector->size = 0;
    vector->capacity = initialCapacity;
}

/***** resizes the vector to a specified new capacity *****/
void VectorInt_Resize(VectorInt* vector, size_t newSize) {
    size_t i;
    
    if (newSize > vector->capacity) {
        size_t newCapacity = (newSize > 2 * vector->capacity) ? newSize : 2 * vector->capacity;
        int* newData = (int*) realloc(vector->data, newCapacity * sizeof(int));
        vector->data = newData;
        vector->capacity = newCapacity;
    }

    for (i = vector->size; i < newSize; ++ i) vector->data[i] = 0;
    vector->size = newSize;
}

/***** inserts a value at a specific index *****/
void VectorInt_Insert(VectorInt* vector, size_t index, int value) {
    if (index >= vector->size) VectorInt_Resize(vector, index + 1);
    vector->data[index] = value;
}

/***** frees allocated memory *****/
void VectorInt_Clear(VectorInt* vector) {
    free(vector->data);
    vector->data = NULL;
    vector->size = 0;
    vector->capacity = 0;
}

/**************************************
    utility functions for VectorStr    
**************************************/

/***** allocates memory *****/
VectorStr* VectorStr_Initialize(void) {
    int initialCapacity = 4;

    VectorStr* vector = (VectorStr*) malloc(sizeof(VectorStr));
    vector->data = (char**) malloc(initialCapacity * sizeof(char*));
    vector->size = 0;
    vector->capacity = initialCapacity;
    return vector;
}

/***** pushes a string to the end *****/
void VectorStr_Push(VectorStr* vector, const char* string) {
    if (vector->size >= vector->capacity) {
        vector->capacity *= 2;
        vector->data = (char**) realloc(vector->data, vector->capacity * sizeof(char*));
    }
    
    vector->data[vector->size] = strdup(string);
    vector->size ++;
}

/***** resizes the vector to a specified new capacity *****/
void VectorStr_Resize(VectorStr* vector, int newSize) {
    int i;

    if (newSize > vector->capacity) {
        vector->capacity = newSize;
        vector->data = (char**) realloc(vector->data, vector->capacity * sizeof(char*));
    }

    for (i = vector->size; i < newSize; i ++) vector->data[i] = strdup("");

    if (newSize < vector->size) {
        int i;
        for (i = newSize; i < vector->size; i ++) free(vector->data[i]);
    }

    vector->size = newSize;
}

/***** inserts a string at a specific index *****/
void VectorStr_Insert(VectorStr* vector, int index, const char* string) {
    if (index >= vector->size) VectorStr_Resize(vector, index + 1);

    if (vector->data[index] != NULL) free(vector->data[index]);
    vector->data[index] = strdup(string);
}

/***** frees allocated memory *****/
void VectorStr_Clear(VectorStr* vector) {
    int i;
    for (i = 0; i < vector->size; i ++) free(vector->data[i]);
    free(vector->data);
    free(vector);
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
VectorPairIntStr* VectorPairIntStr_Initialize() {
    size_t initialCapacity = 4;

    VectorPairIntStr* vector = malloc(sizeof(VectorPairIntStr));
    vector->data = malloc(initialCapacity * sizeof(PairIntStr));
    vector->size = 0;
    vector->capacity = initialCapacity;

    return vector;
}

void _VectorPairIntStr_Resize(VectorPairIntStr* vector) {
    size_t newCapacity = vector->capacity * 2;
    PairIntStr* newData = realloc(vector->data, newCapacity * sizeof(PairIntStr));
    vector->data = newData;
    vector->capacity = newCapacity;
}

/***** pushes an integer-string pair to the end *****/
void VectorPairIntStr_Push(VectorPairIntStr* vector, int first, const char* second) {
    if (vector->size >= vector->capacity) _VectorPairIntStr_Resize(vector);
    vector->data[vector->size].first = first;
    vector->data[vector->size].second = strdup(second);
    vector->size ++;
}

/***** sorts the vector as per the first item in the stored pairs *****/
void VectorPairIntStr_Sort(VectorPairIntStr* vector) {
    qsort(vector->data, vector->size, sizeof(PairIntStr), PairIntStr_Compare);
}

/***** frees allocated memory *****/
void VectorPairIntStr_Clear(VectorPairIntStr* vector) {
    size_t i;
    for (i = 0; i < vector->size; i ++) free(vector->data[i].second);
    free(vector->data);
    free(vector);
}

/***********************************************
    utility functions for MapStrToPairStrInt    
***********************************************/

/***** allocates memory *****/
MapStrToPairStrInt* MapStrToPairStrInt_Initialize(void) {
    size_t initialCapacity = 4;

    MapStrToPairStrInt* map = (MapStrToPairStrInt*) malloc(sizeof(MapStrToPairStrInt));
    map->data = (_MapEntryStrToPairStrInt*) calloc(initialCapacity, sizeof(_MapEntryStrToPairStrInt));
    map->capacity = initialCapacity;
    map->size = 0;
    return map;
}

/***** adds a string-integer pair entry for a given string key to the map *****/
void MapStrToPairStrInt_Add(MapStrToPairStrInt* map, const char* key, const char* first, int second) {
    size_t i;

    if (map->size >= map->capacity) {
        size_t newCapacity = map->capacity * 2;
        _MapEntryStrToPairStrInt* newEntries = (_MapEntryStrToPairStrInt*) realloc(map->data, newCapacity * sizeof(_MapEntryStrToPairStrInt));
        map->data = newEntries;
        map->capacity = newCapacity;
    }

    for (i = 0; i < map->size; i ++) {
        if (strcmp(map->data[i].key, key) == 0) {
            free(map->data[i].value.first);
            map->data[i].value.first = strdup(first);
            map->data[i].value.second = second;
            return;
        }
    }
    
    map->data[map->size].key = strdup(key);
    map->data[map->size].value.first = strdup(first);
    map->data[map->size].value.second = second;
    map->size ++;
}

/***** returns the string-integer pair corresponding to a given searchKey *****/
PairStrInt* MapStrToPairStrInt_Find(MapStrToPairStrInt* map, const char* key) {
    size_t i;
    for (i = 0; i < map->size; i ++) if (strcmp(map->data[i].key, key) == 0) return &map->data[i].value;
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
MapStrToInt* MapStrToInt_Initialize(void) {
    size_t initialCapacity = 4;

    MapStrToInt* map = (MapStrToInt*) malloc(sizeof(MapStrToInt));
    map->data = (_MapEntryStrToInt*) malloc(initialCapacity * sizeof(_MapEntryStrToInt));
    map->size = 0;
    map->capacity = initialCapacity;
    return map;
}

/***** frees allocated memory *****/
void MapStrToInt_Clear(MapStrToInt* map) {
    size_t i;
    for (i = 0; i < map->size; i ++) free(map->data[i].key);
    free(map->data);
    map->data = NULL;
    map->size = 0;
    map->capacity = 0;
}

/***** adds an integer entry for a given string key to the map *****/
void MapStrToInt_Add(MapStrToInt* map, const char* key, int value) {
    size_t i;

    if (map->size >= map->capacity) {
        size_t newCapacity = map->capacity * 2;
        _MapEntryStrToInt* newData = (_MapEntryStrToInt*) realloc(map->data, newCapacity * sizeof(_MapEntryStrToInt));
        map->data = newData;
        map->capacity = newCapacity;
    }

    for (i = 0; i < map->size; i ++) {
        if (strcmp(map->data[i].key, key) == 0) {
            map->data[i].value = value;
            return;
        }
    }
    
    map->data[map->size].key = strdup(key);
    map->data[map->size].value = value;
    map->size ++;
}

/***** returns a pointer to the integer corresponding to a given searchKey *****/
int* MapStrToInt_Find(MapStrToInt* map, const char* key) {
    size_t i;
    for (i = 0; i < map->size; i ++) if (strcmp(map->data[i].key, key) == 0) return &map->data[i].value;
    return NULL;
}

/************************************************
    utility functions for VectorListingCustom    
************************************************/

/***** allocates memory *****/
void VectorListingCustom_Initialize(VectorListingCustom* vector) {
    size_t initialCapacity = 4;
    
    size_t i;
    
    vector->size = 0;
    vector->capacity = initialCapacity;
    vector->data = (ListingCustom*) malloc(sizeof(ListingCustom) * initialCapacity);

    for (i = 0; i < vector->capacity; i ++) {
        vector->data[i].label = strdup("");
        vector->data[i].mnemonic = strdup("");
        vector->data[i].operand = strdup("");
    }
}

/***** resizes the vector to a specified new capacity *****/
void VectorListingCustom_Resize(VectorListingCustom* vector, size_t newCapacity) {
    size_t i;

    ListingCustom* newData;

    if (newCapacity <= vector->capacity) return;

    newData = (ListingCustom*) realloc(vector->data, sizeof(ListingCustom) * newCapacity);
    for (i = vector->capacity; i < newCapacity; i ++) {
        newData[i].label = strdup("");
        newData[i].mnemonic = strdup("");
        newData[i].operand = strdup("");
    }
    vector->data = newData;
    vector->capacity = newCapacity;
}

void VectorListingCustom_Insert(VectorListingCustom* vector, const void* fieldValue, size_t index, const char* fieldName) {
    ListingCustom* entry = &vector->data[index];
    
    if (index >= vector->size) {
        vector->size = index + 1;
        if (vector->size > vector->capacity) VectorListingCustom_Resize(vector, vector->capacity * 2);
    }
    
    if (strcmp(fieldName, "label") == 0) {
        if (!strcmp(entry->label, "")) free(entry->label);
        entry->label = strdup((char*) fieldValue);
    } else if (strcmp(fieldName, "mnemonic") == 0) {
        if (!strcmp(entry->mnemonic, "")) free(entry->mnemonic);
        entry->mnemonic = strdup((char*) fieldValue);
    } else if (strcmp(fieldName, "operand") == 0) {
        if (!strcmp(entry->operand, "")) free(entry->operand);
        entry->operand = strdup((char*) fieldValue);
    } else if (strcmp(fieldName, "operandType") == 0) entry->operandType = *(int*) fieldValue;
    else if (strcmp(fieldName, "isLabelPresent") == 0) entry->isLabelPresent = *(bool*) fieldValue;
}

/***** frees allocated memory *****/
void VectorListingCustom_Clear(VectorListingCustom* vector) {
    size_t i;
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