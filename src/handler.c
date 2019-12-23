#include <dirent.h>
#include <stdio.h>
#include "handler.h"
#include "request.h"
#include "response.h"
#include "utility.h"
#include "kv.h"

Response *staticHandler(Request *req)
{
    Response *response = responseNew();
    char *filename = (req->path) + 1;
    if (strlen(req->path) == 1 && !strncmp(req->path, "/", 1)) return NULL; // req->path == "/"
    if (isDir(filename)) return NULL;
    if (!isFile(filename)) {
        responseSetStatus(response, FORBIDDEN);
        responseSetBody(response, "<h1>Forbidden</h1><p>File is missing</p>");
        responseSetContentLength(response, 39);
        return response;
    }
    responseSetBody(response, readfile(filename));
    responseSetContentLength(response, fileLength(filename));
    responseAddHeader(response, kvNew("Content-Type", findMimeType(filename)));
    return response;
}

Response *directoryHandler(Request *req)
{
    Response *response = responseNew();
    DIR* dir;
    struct dirent *ptr;

    if (!strncmp(req->path, "/", 1)){
        char cwd[256];
        getcwd(cwd, sizeof(cwd));
        dir = opendir(cwd);
    } else dir = opendir(req->path + 1);
    
    if (dir) {
        char *indexPath = concat(req->path+1, "/index.html");
        printString(indexPath);
        if (isFile(indexPath)) {
            responseSetBody(response, readfile(indexPath));
            responseSetContentLength(response, fileLength(indexPath));
            responseAddHeader(response, kvNew("Content-Type", findMimeType(indexPath)));
            return response;
        }
        responseSetBody(response, "<h1>List of files should be implemented.</h1>");
        responseSetContentLength(response, 39);
    } else return NULL;
    return response;
}