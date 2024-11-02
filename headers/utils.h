#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>

char* readLine(FILE* filePtr); /***** reads a line from a file and returns it *****/

char *strdup(const char *str); /***** duplicates a string and returns it *****/

void strrev(char* str); /***** reverses a string in-place *****/

char* substr(const char* string, int start, int length); /***** returns a slice of a string *****/

int minOf2Ints(int a, int b); /***** returns the minimum of two integers *****/

#endif