#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include "utility.h"
#include "list.h"

void changePrintColor(char *color)
{
    if (!strncmp(color, "red", 3)) printf("\033[0;31m");
    else if (!strncmp(color, "bold-red", 8)) printf("\033[1;31m");
    else if (!strncmp(color, "green", 5)) printf("\033[0;32m");
    else if (!strncmp(color, "bold-green", 10)) printf("\033[1;32m");
    else if (!strncmp(color, "yellow", 6)) printf("\033[0;33m");
    else if (!strncmp(color, "bold-yellow", 11)) printf("\033[01;33m");
    else if (!strncmp(color, "blue", 4)) printf("\033[0;34m");
    else if (!strncmp(color, "bold-blue", 9)) printf("\033[1;34m");
    else if (!strncmp(color, "magenta", 7)) printf("\033[0;35m");
    else if (!strncmp(color, "bold-magenta", 12)) printf("\033[1;35m");
    else if (!strncmp(color, "cyan", 4)) printf("\033[0;36m");
    else if (!strncmp(color, "bold-cyan", 9)) printf("\033[1;36m");
    else if (!strncmp(color, "white", 5)) printf("\033[0m");
    else printf("\033[0m");
    fflush(stdout);
}

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
    close(file);
    return buff;
}

int isFileReadable(char *filename)
{
    FILE *file = fopen(filename, "r");
    if (file) {
        close(file);
        return 1;
    } else {
        close(file);
        return 0;    
    }
}

int isFile(char *filename)
{
    struct stat s, ls;
    lstat(filename, &ls);
    stat(filename, &s);
    return S_ISREG(s.st_mode) == 1 || S_ISLNK(ls.st_mode) == 1;
}

int isDir(char *filename)
{
    struct stat s;
    stat(filename, &s);
    return S_ISDIR(s.st_mode) == 1;
}

int isDirReadable(char *filename)
{
    DIR *dir = opendir(filename);
    if (dir == NULL) {
        closedir(dir);
        return 0;
    } else {
        closedir(dir);
        return 1;
    }
}

char *concat(const char *s1, const char *s2)
{
    char *result = malloc(strlen(s1) + strlen(s2) + 1);
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}

char * subString(char *string, int start, int end)
{
    char *substring = malloc(sizeof(char) * (end - start) );
    for (int i = 0; i < strlen(string) ; i++) {
        strncpy(substring, string + start, end - start);
    }
    return substring;
}

char * intToString(int num)
{
    char *str = malloc(sizeof(char) * 64);
    sprintf(str,"%d",num);
    return str;
}

char * execCommand(char *command)
{
    int fd[2];
    pipe(fd);
    pid_t pid = fork();
    if (pid == 0){
        close(fd[0]);
        dup2(fd[1], 1);
        system(command);
        exit(0);
    } else if (pid > 0) {
        close(fd[1]);
        size_t size = 20480;
        char *buffer = malloc(size);
        memset(buffer, 0, size);
        read(fd[0], buffer, size);
        close(fd[0]);
        return buffer;
    }
}