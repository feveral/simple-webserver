#include <dirent.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "handler.h"
#include "request.h"
#include "response.h"
#include "string.h"
#include "utility.h"
#include "kv.h"

static char *execLS(char *path)
{
    char *command = concat("ls -al ", path);
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
        size_t size = 10240;
        char *buffer = malloc(size);
        memset(buffer, 0, size);
        read(fd[0], buffer, size);
        close(fd[0]);
        return buffer;
    }
}

char *stringWithATag(char *string)
{
    List *items = split(string, "/");
    char *last = listGet(items, (items->count)-1)->value;
    char *html = malloc(2048);
    memset(html, 0, strlen(string) + strlen(last) + 20);
    sprintf(html, "<a href=\"/%s\">%s</a>\n", string, last);
    return html;
}

char *lsWithHTML(char *lsresult, char *path)
{
    List *lines = split(lsresult, "\n");
    char *result = malloc(4096);
    memset(result, 0 , 4096);
    int count = 0;
    sprintf(result, "<meta http-equiv=\"content-type\" content=\"text/html; charset=utf-8\">");
    count += 67;
    sprintf(result+count, "<style>body {\nfont-family: monospace;\nwhite-space: pre;\n}</style><hr>");
    count += 69;
    for(int i = 1; i < lines->count; i++) {
        List *items = split(listGet(lines, i)->value, " ");
        for(int j = 0; j < 8; j++) {
            char *item = listGet(items, j)->value;
            if (j == 0) {
                sprintf(result+count, "%-12s", item);
                count += 12;
            } else {
                sprintf(result+count, "%-9s", item);
                count += 9;
            }
        }
        char *pathwithslash;
        if (path[strlen(path)-1] != '/')
            pathwithslash = concat(path+1, "/");
        else pathwithslash = concat(path+1, "");

        if (!strncmp(pathwithslash, "/", 1)) pathwithslash = concat("", "");
        char *filepath = concat(pathwithslash, listGet(items, 8)->value);
        char *html = stringWithATag(filepath);
        sprintf(result+count, "%s", html);
        count += strlen(html);
        free(pathwithslash);
        free(filepath);
        free(html);
        listFree(items);
    }
    sprintf(result+count, "<hr>");
    count += 4;
    listFree(lines);
    return result;
}

Response *staticHandler(Request *req)
{
    char *filename = (req->path) + 1;
    if (!strncmp(req->path, "/", strlen(req->path))) return NULL; // req->path == "/"
    if (isDir(filename)) return NULL;
    if (isFile(filename) && !isFileReadable(filename)) return response404(filename);
    if (!isFile(filename)) return response403(filename);
    return responseStaticFile(filename);
}

Response *directoryHandler(Request *req)
{
    char *dirpath;
    if (!strncmp(req->path, "/", strlen(req->path))){
        dirpath = "./";
    } else dirpath = req->path + 1;

    if (isDir(dirpath)) {
        if (req->path[strlen(req->path)-1] != '/') {
            return response301(req->path, concat(req->path, "/"));
        }
        Response *response = responseIndex(req->path+1);
        if (response) return response;
        else response = responseNew();
        char *lsResult = execLS(req->path + 1);
        lsResult = lsWithHTML(lsResult, req->path);
        responseSetBody(response, lsResult);
        responseAddHeader(response, kvNew("Content-Type", "text/html"));
        responseSetContentLength(response, strlen(lsResult));
        return response;
    } else return NULL;
}

Response *cgiHandler(Request *req)
{
    if(!strncmp(req->path, "/", strlen(req->path))) return NULL;
    List *dotSplits = split(req->path, ".");
    List *slashSplits = split(req->path, "/");
    char *ext = listGet(dotSplits, (dotSplits->count)-1)->value;
    char *parentdirname = listGet(slashSplits, (slashSplits->count)-2)->value;
    char *input = "";

    if (strncmp(ext, "cgi", 3) && strstr(parentdirname, "cgi") == 0 ) { listFree(dotSplits); return NULL; }
    if (req->method == POST) input = req->body;

    int fd[2];
    pipe(fd);
    pid_t pid = fork();
    if (pid == 0) {
        char *command = malloc(1024);
        memset(command, 0, 1024);
        for (int i = 0; i < req->qslist->count; i++) {
            KV *kv = listGet(req->qslist, i)->value;
            strcat(command, kv->key);
            strcat(command, "=");
            strcat(command, kv->value);
        }
        strcat(command, "/bin/bash -c \".");
        strcat(command, req->path);
        strcat(command, " <<< \'");
        strcat(command, req->body);
        strcat(command, "\'\"");
        dup2(fd[1], 1);
        system(command);
        close(fd[0]);
        close(fd[1]);
        exit(0);
    } else if (pid > 0) {
        size_t contentLength = 0;
        size_t size = 20480;
        char buffer[size];
        memset(&buffer, 0, size);
        fcntl(fd[0], F_SETFL, fcntl(fd[0], F_GETFL) | O_NONBLOCK);
        wait();
        while(   (read(fd[0], buffer+contentLength, 1)) > 0) { 
            contentLength += 1;
        }
        close(fd[0]);
        close(fd[1]);
        Response *response = responseNew();
        responseSetBody(response, buffer);
        responseAddHeader(response, kvNew("Content-Type", "text/plain"));
        responseSetContentLength(response, contentLength);
        return response;
    }
}
