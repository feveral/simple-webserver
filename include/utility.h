#ifndef UTILITY_H
#define UTILITY_H

#include "list.h"

void printString(char *string);
List *split(char *string, const char *token);
char * concat(const char *s1, const char *s2);
char *readfile(char *filename);

#endif