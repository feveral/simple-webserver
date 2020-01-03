#ifndef REQUEST_H
#define REQUEST_H

#include "list.h"

typedef enum Method {
    GET, POST, OTHER
} Method;

typedef struct request {
    Method method;
    char *path;
    List *qslist; // List of kv
    char *body;
} Request;

void printRequest(Request *request);
Method toMethod(char *string);
char *methodToString(Method method);
Request *HttpRawPacketToRequest(char *packet);
List *queryStringNew(char *queryString);
Request *requestNew(Method method, char *path, char *queryString);
void freeRequest(Request *request);
#endif