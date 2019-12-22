#ifndef RESPONSE_H
#define RESPONSE_H

#include "list.h"

typedef enum Status {
    OK = 200,
    MOVE_PERMANENTLY = 301,
    FORBIDDEN = 403,
    NOT_FOUND = 404,
} Status;

typedef struct response {
    Status status;
    List *headers; // List of kv
    char *body;
} Response;

Response *responseNew();
void responseAddHeader(Response *, char*);
void responseSetStatus(Response *, Status);
void responseSetBody(Response*, char*);
char* responsePacket(Response *response);

#endif