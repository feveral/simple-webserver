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

char *readfile(char *filename)
{
    FILE *file = fopen(filename, "r");

    char *buff;
    char  lens[25];
    size_t len;

    fseek(file, 0, SEEK_END);
    len = ftell(file);
    sprintf(lens, "%ld", (long int) len);
    rewind(file);

    buff = malloc(sizeof(char) * len);
    fread(buff, sizeof(char), len, file);
    return buff;
}

char * concat(const char *s1, const char *s2)
{
    char *result = malloc(strlen(s1) + strlen(s2) + 1);
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}