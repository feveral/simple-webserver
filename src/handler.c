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

char *stringWithATag(char *string)
{
    char *html = malloc(2*strlen(string) + 20);
    memset(html, 0, 2*strlen(string) + 20);
    memcpy(html, "<a href=\"./", 11);
    memcpy(html+11, string, strlen(string));
    memcpy(html+11+strlen(string), "\">", 2);
    memcpy(html+11+strlen(string)+2, string, strlen(string));
    memcpy(html+11+2*strlen(string)+2, "</a>\n", 5);
    return html;
}

char *lsWithHTML(char *lsresult, char *path)
{
    List *lines = split(lsresult, "\n");
    char *result = malloc(4096);
    memset(result, 0 , 4096);
    int count = 0;
    memcpy(result, "<meta http-equiv=\"content-type\" content=\"text/html; charset=utf-8\">", 67);
    count += 67;
    memcpy(result+count, "<style>body {\nfont-family: monospace;\nwhite-space: pre;\n}</style><hr>", 69);
    count += 69;
    for(int i = 1; i < lines->count; i++) {
        List *items = split(listGet(lines, i)->value, " ");
        for(int j = 0; j < 8; j++) {
            char *item = listGet(items, j)->value;
            memcpy(result+count, item, strlen(item));
            count += strlen(item);
            memcpy(result+count, "    ", 4);
            count += 4;
        }
        char *pathwithslash = concat(path+1, "/");
        if (!strncmp(pathwithslash, "/")) pathwithslash = concat("", "");
        char *filepath = concat(pathwithslash, listGet(items, 8)->value);
        char *html = stringWithATag(filepath);
        memcpy(result+count, html, strlen(html));
        count += strlen(html);
        free(pathwithslash);
        free(filepath);
        free(html);
    }
    memcpy(result+count, "<hr>", 4);
    printString(result);
    return result;
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
        char *lsResult = execLS(req->path + 1);
        lsResult = lsWithHTML(lsResult, req->path);
        responseSetBody(response, lsResult);
        responseAddHeader(response, kvNew("Content-Type", "text/html"));
        responseSetContentLength(response, strlen(lsResult));
        return response;
    } else return NULL;
}