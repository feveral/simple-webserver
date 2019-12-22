#include <stdio.h>
#include <string.h>
#include "utility.h"
#include "list.h"

void printString(char *string)
{
    printf("%s\n", string);
}

List *split(char *string, const char *token)
{
    List *list = listNew();
    int bufferLength = strlen(string)+1;
    char buffer[bufferLength];

    memset(buffer, 0, bufferLength);
    strncpy(buffer, string, bufferLength-1);
    buffer[bufferLength-1] = '\0';

    char *fragment = strtok(buffer, token);
    listAppendString(list, fragment);
    while(fragment != NULL) {
        fragment = strtok(NULL, token);
        if (fragment != NULL) {
            listAppendString(list, fragment);
        }
    }
    return list;
}

size_t fileLength(char *filename)
{
    FILE *file = fopen(filename, "r");
    size_t length;
    fseek(file, 0, SEEK_END);
    length = ftell(file);
    rewind(file);
    return length;
}

char *readfile(char *filename)
{
    FILE *file = fopen(filename, "r");
    char *buff;
    size_t length = fileLength(filename);
    buff = malloc(sizeof(char) * length);
    fread(buff, sizeof(char), length, file);
    return buff;
}

char *concat(const char *s1, const char *s2)
{
    char *result = malloc(strlen(s1) + strlen(s2) + 1);
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}