#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**********************************************
    reads a line from a file and returns it    
**********************************************/
char* readLine(FILE* filePtr) {
    size_t len = 0;
    size_t capacity = 128;

    char* buffer = malloc(capacity);

    int ch;
    while ((ch = fgetc(filePtr)) != EOF && ch != '\n') {
        if (len + 1 >= capacity) {
            char* temp = NULL;
            
            capacity *= 2;
            temp = realloc(buffer, capacity);
            buffer = temp;
        }
        buffer[len ++] = ch;
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
char* strdup(const char *str) {
    char* dup = malloc(strlen(str) + 1);
    if (dup == NULL) return NULL;
    strcpy(dup, str);
    return dup;
}

/*********************************
    reverses a string in-place    
*********************************/
void strrev(char* str) {
    int i = 0;
    int j = strlen(str) - 1;
    while (i < j) {
        char temp = str[i];
        str[i] = str[j];
        str[j] = temp;
        i ++; j --;
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

/**********************************************************************
    converts a decimal integer to a hexadecimal string in assembler    
**********************************************************************/
char* decimalToHexVA(int number, int add) { /* default add = 24 */
    static char hexString[33];

    if (add == 32) {
        unsigned int num = number;
        sprintf(hexString, "%x", num);
        return hexString;
    }

    if (number < 0) number += (1 << add);

    sprintf(hexString, "%x", number);
    return hexString;
}

/*********************************************************************
    converts a decimal integer to a hexadecimal string in emulator    
*********************************************************************/
char* decimalToHexVE(unsigned int number) {
    const char hexDigits[] = "0123456789abcdef";
    char* converted = (char*) malloc(9 * sizeof(char));

    int i = 7;

    int j;
    for (j = 0; j < 8; j ++) converted[j] = '0';
    converted[8] = '\0';

    while (number != 0 && i >= 0) {
        converted[i] = hexDigits[number % 16];
        number /= 16;
        i --;
    }
    return converted;
}

/****************************************************************************
    pads the start of a string with 0s to make a size-bit string of numbers    
****************************************************************************/
const char* padWithZero(char* numStr, int size) { /* default size = 6 */
    static char paddedNumStr[33];

    unsigned int i;

    strrev(numStr);

    for (i = 0; i < strlen(numStr); i ++) paddedNumStr[i] = numStr[i];
    paddedNumStr[strlen(numStr)] = '\0';

    while (strlen(paddedNumStr) < (unsigned) size) strcat(paddedNumStr, "0");

    strrev(numStr);
    strrev(paddedNumStr);
    return paddedNumStr;
}

/***********************************************************
    returns whether a string represents a decimal number    
***********************************************************/
bool isDecimal(char* s) {
    bool verdict = true;

    unsigned int i;
    for (i = 0; i < strlen(s); i ++) verdict &= (s[i] >= '0' && s[i] <= '9');
    return verdict;
}

/**********************************************************
    returns whether a string represents an octal number    
**********************************************************/
bool isOctal(char* s) {
    bool verdict = true;

    unsigned int i;

    if (strlen(s) < 3) return false;
    
    for (i = 2; i < strlen(s); i ++) verdict &= (s[i] >= '0' && s[i] <= '7');
    return verdict & (s[0] == '0' && (s[1] == 'o' || s[1] == 'O'));
}

/***************************************************************
    returns whether a string represents a hexadecimal number    
***************************************************************/
bool isHexadecimal(char* s) {
    bool verdict = true;

    unsigned int i;

    if (strlen(s) < 3) return false;

    for (i = 2; i < strlen(s); i ++) verdict &= ((s[i] >= '0' && s[i] <= '9') || (s[i] >= 'a' && s[i] <= 'f') || (s[i] >= 'A' && s[i] <= 'F'));
    return verdict & (s[0] == '0' && (s[1] == 'x' || s[1] == 'X'));
}