#ifndef REQUEST_H
#define REQUEST_H

#include "list.h"

typedef enum Method {
    GET, POST, OTHER
} Method;

typedef struct request {
    Method method;
    char *uri;
    char *clientIP;
    int clientPort;
    List *headers;
    char *path;
    char *queryString;
    List *qslist; // processed querystring, is a List of type KV
    char *body;
} Request;

void printRequest(Request *request);
Method toMethod(char *string);
char *methodToString(Method method);
Request *requestNew(char *packet, struct sockaddr_in *sin);
List *queryListNew(char *queryString);
List *headerListNew(char *packet);
char *requestGetHeader(Request *request, char *name);
void freeRequest(Request *request);
#endif