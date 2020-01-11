#include <dirent.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include "handler.h"
#include "request.h"
#include "response.h"
#include "string.h"
#include "utility.h"
#include "kv.h"

char *stringWithATag(char *string)
{
    List *items = split(string, "/");
    char *last = listGet(items, (items->count)-1);
    char *html = malloc(2048);
    memset(html, 0, 2048);
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
        List *items = split(listGet(lines, i), " ");
        for(int j = 0; j < 8; j++) {
            char *item = listGet(items, j);
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
        char *filepath = concat(pathwithslash, listGet(items, 8));
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
    if (isFile(filename) && !isFileReadable(filename)) return response403(filename);
    if (!isFile(filename)) return response404(req->path);
    return responseStaticFile(filename);
}

Response *directoryHandler(Request *req)
{
    char *dirpath;
    if (!strncmp(req->path, "/", strlen(req->path))){
        dirpath = "./";
    } else dirpath = req->path + 1;

    if (isDir(dirpath) && isDirReadable(dirpath)) {
        if (req->path[strlen(req->path)-1] != '/') {
            return response301(req->path, concat(req->path, "/"));
        }
        Response *response = responseIndex(req->path+1);
        if (response) return response;
        else response = responseNew();
        char *lsResult = execCommand(concat("ls -al ", req->path + 1));
        lsResult = lsWithHTML(lsResult, req->path);
        responseSetBody(response, lsResult);
        responseAddHeader(response, kvNew("Content-Type", "text/html"));
        responseSetContentLength(response, strlen(lsResult));
        return response;
    } else if (isDir(dirpath) && !isDirReadable(dirpath)) {
        return response404(dirpath);
    }
    return NULL;
}

Response *cgiHandler(Request *req)
{
    if(!strncmp(req->path, "/", strlen(req->path))) return NULL;
    List *dotSplits = split(req->path, ".");
    List *slashSplits = split(req->path, "/");
    char *ext = listGet(dotSplits, (dotSplits->count)-1);
    char *parentdirname = listGet(slashSplits, (slashSplits->count)-2);
    char *input = "";

    if (strncmp(ext, "cgi", 3) && strstr(parentdirname, "cgi") == 0 ) { listFree(dotSplits); return NULL; }
    if (req->method == POST) input = req->body;

    char *method_env = concat("REQUEST_METHOD=", methodToString(req->method));
    char *uri_env = concat("REQUEST_URI=", req->uri);
    char *content_length_env = concat("CONTENT_LENGTH=", requestGetHeader(req, "Content-Length"));
    char *content_type_env = concat("CONTENT_TYPE=", requestGetHeader(req, "Content-Type"));
    char *script_name = concat("SCRIPT_NAME=", req->path);
    char *qsenv = concat("QUERY_STRING=", req->queryString);
    char *gateway_env = "GATEWAY_INTERFACE=CGI/1.1";
    char *remote_addr_env = concat("REMOTE_ADDR=", req->clientIP);
    char *remote_port_env = concat("REMOTE_PORT=", intToString(req->clientPort));
    char *path_env = "PATH=/bin:/usr/bin:/usr/local/bin";

    putenv(method_env);
    putenv(uri_env);
    putenv(script_name);
    putenv(qsenv);
    putenv(gateway_env);
    putenv(remote_addr_env);
    putenv(remote_port_env);
    putenv(path_env);

    if (req->method == POST) putenv(content_type_env);
    if (req->method == POST) putenv(content_length_env);

    char *command = malloc(1024);
    memset(command, 0, 1024);
    strcat(command, "/bin/bash -c \".");
    strcat(command, req->path);
    strcat(command, " <<< \'");
    strcat(command, req->body);
    strcat(command, "\'\"");

    int fd[2];
    pipe(fd);
    pid_t pid = fork();
    if (pid == 0) {
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
