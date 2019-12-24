#include <dirent.h>
#include <stdio.h>
#include "handler.h"
#include "request.h"
#include "response.h"
#include "utility.h"
#include "kv.h"

static char *execLS(char *path)
{
    char *command = concat("ls -al ", path);
    int fd[2];
    pipe(fd);
    pid_t pid = fork();
    if (pid == 0){
        dup2(fd[1], 1);
        system(command);
        exit(0);
    }
    else if (pid > 0) {
        size_t size = 10240;
        char *buffer = malloc(size);
        memset(buffer, 0, size);
        read(fd[0], buffer, size);
        return buffer;
    }
}

char *lsWithATag(char *lsresult)
{
    List *lines = split(lsresult, "\n");
    for(int i = 0; i < lines->count; i++) {
        List *items = split(listGet(lines, i)->value, " ");
    }
}

Response *staticHandler(Request *req)
{
    char *filename = (req->path) + 1;
    if (!strncmp(req->path, "/", strlen(req->path))) return NULL; // req->path == "/"
    if (isDir(filename)) return NULL;
    if (!isFile(filename)) return response403(filename);
    return responseStaticFile(filename);
}

Response *directoryHandler(Request *req)
{
    DIR* dir;
    struct dirent *ptr;

    if (!strncmp(req->path, "/", strlen(req->path))){
        dir = opendir("./");
    } else dir = opendir(req->path + 1);
    
    if (dir) {
        closedir(dir);
        Response *response = responseIndex(req->path+1);
        if (response) return response;
        else response = responseNew();
        // responseSetBody(response, "<h1>List of files should be implemented.</h1>");
        // responseSetContentLength(response, 45);
        char *lsResult = execLS(req->path + 1);
        responseSetBody(response, lsResult);
        responseSetContentLength(response, strlen(lsResult));
        return response;
    } else return NULL;
}