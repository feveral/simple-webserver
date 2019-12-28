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
        close(fd[0]);
        dup2(fd[1], 1);
        system(command);
        exit(0);
    }
    else if (pid > 0) {
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
        char *pathwithslash = concat(path+1, "/");
        if (!strncmp(pathwithslash, "/")) pathwithslash = concat("", "");
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
    sprintf(result+count, "<h3 style=\"text-align:center;\">You can not hack me!</h3>");
    listFree(lines);
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

Response *cgiHandler(Request *req)
{
    List *dotSplits = split(req->path, ".");
    char *ext = (listGet(dotSplits, (dotSplits->count)-1)->value);
    if (strncmp(ext, "cgi")) { listFree(dotSplits); return NULL; }

    int fd[2];
    pipe(fd);
    pid_t pid = fork();
    if (pid == 0) {
        close(fd[0]);
        char *argv[ ]={"./testcase/printenv_sh.cgi", 0}; 
        dup2(fd[1], 1);
        char *envp[]={0,NULL};
        execve("./testcase/printenv_sh.cgi", argv, NULL);
        exit(0);
    } else if (pid > 0) {

        size_t contentLength = 0;
        size_t size = 20480;
        char buffer[size];
        memset(buffer, 0, size);

        size_t len = 0;
        char buf[1024];
        memset(&buf, 0, 1024);
        // while((len = read(fd[0], buf, sizeof(buf))) > 0) {
        //     printf("%d\n", len);
        //     memcpy(buffer+contentLength, buf, len);
        //     contentLength += len;
        // }
        // len = read(fd[0], buf, sizeof(buf));
        // contentLength += len;
        // memcpy(buffer, buf, len);
        // len = read(fd[0], buf, sizeof(buf));
        // contentLength += len;
        // memcpy(buffer+len, buf, len);

        // printf("%d\n", contentLength);

        // contentLength = read(fd[0], buffer, size);

        for (int i = 0 ; i < contentLength ; i++) {
            printf("%c", buffer[i]);
        }
        Response *response = responseNew();
        responseSetBody(response, buffer);
        responseAddHeader(response, kvNew("Content-Type", "text/plain"));
        responseSetContentLength(response, contentLength);
        return response;
    }
}