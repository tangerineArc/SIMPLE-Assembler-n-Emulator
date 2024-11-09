/*********************************************************************
------------------- DECLARATION OF AUTHORSHIP ------------------------
I hereby declare that the source-code presented herein is my own work.
    --- Author Name: SWAGATAM PATI
    --- Roll No.: 2301AI28
----------------------------------------------------------------------
*********************************************************************/

#ifndef DATA_STRUCTURES_H
#define DATA_STRUCTURES_H

#include <stdbool.h>
#include <stdio.h>

/********************************************
    data-structure for vector of integers    
********************************************/
typedef struct {
    int* data;
    size_t size;
    size_t capacity;
} VectorInt;

void VectorInt_Initialize(VectorInt* vector); /* allocates memory */
void VectorInt_Resize(VectorInt* vector, size_t newSize); /* resizes the vector to a specified new capacity */
void VectorInt_Insert(VectorInt* vector, size_t index, int value); /* inserts a value at a specific index */
void VectorInt_Clear(VectorInt* vector); /* frees allocated memory */

/*******************************************
    data-structure for vector of strings    
*******************************************/
typedef struct {
    char** data;
    int size;
    int capacity;
} VectorStr;

VectorStr* VectorStr_Initialize(void);
void VectorStr_Push(VectorStr* vector, const char* string); /* pushes a string to the end */
void VectorStr_Resize(VectorStr* vector, int newCapacity); /* resizes the vector to a specified new capacity */
void VectorStr_Insert(VectorStr* vector, int index, const char* string); /* inserts a string at a specific index */
void VectorStr_Clear(VectorStr* vector); /* frees allocated memory */

/**********************************************
    data-structure for string-integer pairs    
**********************************************/
typedef struct {
    char* first;
    int second;
} PairStrInt;

/**********************************************
    data-structure for integer-string pairs    
**********************************************/
typedef struct {
    int first;
    char* second;
} PairIntStr;

int PairIntStr_Compare(const void* a, const void* b); /* comapres two PairIntStr's a and b; return a->first - b->first */

/********************************************************
    data-structure for vector of integer-string pairs    
********************************************************/
typedef struct {
    PairIntStr* data;
    size_t size;
    size_t capacity;
} VectorPairIntStr;

VectorPairIntStr* VectorPairIntStr_Initialize(void); /* allocates memory */
void VectorPairIntStr_Push(VectorPairIntStr* vector, int first, const char* second); /* pushes an integer-string pair to the end */
void VectorPairIntStr_Sort(VectorPairIntStr* vector); /* sorts the vector as per the first item in the stored pairs */
void VectorPairIntStr_Clear(VectorPairIntStr* vector); /* frees allocated memory */

/****************************************************************
    data-structure for map of strings to string-integer pairs    
****************************************************************/
typedef struct {
    char* key;
    PairStrInt value;
} _MapEntryStrToPairStrInt;

typedef struct {
    _MapEntryStrToPairStrInt* data;
    size_t capacity;
    size_t size;
} MapStrToPairStrInt;

MapStrToPairStrInt* MapStrToPairStrInt_Initialize(void); /* allocates memory */
void MapStrToPairStrInt_Add(MapStrToPairStrInt* map, const char* key, const char* first, int second); /* adds a string-integer pair entry for a given string key to the map */
PairStrInt* MapStrToPairStrInt_Find(MapStrToPairStrInt* map, const char* key); /* returns the string-integer pair corresponding to a given searchKey */
void MapStrToPairStrInt_Clear(MapStrToPairStrInt* map); /* frees allocated memory */

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

MapStrToInt* MapStrToInt_Initialize(void); /* allocates memory */
void MapStrToInt_Clear(MapStrToInt* map); /* frees allocated memory */
void MapStrToInt_Add(MapStrToInt* map, const char* key, int value); /* adds an integer entry for a given string key to the map */
int* MapStrToInt_Find(MapStrToInt* map, const char* key); /* returns a pointer to the integer corresponding to a given searchKey */

/***************************************************************************
    custom data-structure for listing of 3 strings, 1 integer, 1 boolean    
***************************************************************************/
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
    size_t size;
    size_t capacity;
} VectorListingCustom;

void VectorListingCustom_Initialize(VectorListingCustom* vector); /* allocates memory */
void VectorListingCustom_Resize(VectorListingCustom* vector, size_t newCapacity); /* resizes the vector to a specified new capacity */
void VectorListingCustom_Insert(VectorListingCustom* vector, const void* fieldValue, size_t index, const char* fieldName); /* inserts a specific field at a specified index */
void VectorListingCustom_Clear(VectorListingCustom* vector); /* frees allocated memory */

#endif
