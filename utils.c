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