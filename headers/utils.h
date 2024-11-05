#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdbool.h>

char* readLine(FILE* filePtr); /* reads a line from a file and returns it */

char* strdup(const char *str); /* duplicates a string and returns it */
void strrev(char* str); /* reverses a string in-place */
char* substr(const char* string, int start, int length); /* returns a slice of a string */

int minOf2Ints(int a, int b); /* returns the minimum of two integers */

char* decimalToHexVA(int number, int add); /* converts a decimal integer to a hexadecimal string in assembler */
char* decimalToHexVE(unsigned int number); /* converts a decimal integer to a hexadecimal string in emulator */

char* padWithZero(char* numStr, int size); /* pads the start of a string with 0s to make a size-bit string of numbers */

bool isDecimal(char* s); /* returns whether a string represents a decimal number */
bool isOctal(char* s); /* returns whether a string represents an octal number */
bool isHexadecimal(char* s); /* returns whether a string represents a hexadecimal number */

#endif