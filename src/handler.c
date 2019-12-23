#include <dirent.h>
#include <stdio.h>
#include "handler.h"
#include "request.h"
#include "response.h"
#include "utility.h"
#include "kv.h"

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
        responseSetBody(response, "<h1>List of files should be implemented.</h1>");
        responseSetContentLength(response, 45);
        return response;
    } else return NULL;
}