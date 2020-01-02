#ifndef UTILITY_H
#define UTILITY_H

#include "list.h"

void changePrintColor(char *color);
void printString(char *string);
List *split(char *string, const char *token);
char * concat(const char *s1, const char *s2);
size_t fileLength(char *filename);
char *readfile(char *filename);
int isFileReadable(char *filename);
int isFile(char *filename);
int isDir(char *filename);

#endif