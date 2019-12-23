#ifndef RESPONSE_H
#define RESPONSE_H

#include "list.h"
#include "kv.h"

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
    size_t statusLength;
    size_t headerLength;
    size_t contentLength;
} Response;

Response *responseNew();
void responseAddHeader(Response *, KV*);
void responseSetStatus(Response *, Status);
void responseSetBody(Response*, char*);
char* responsePacket(Response *response);
void responseSetStatusLength(Response *response, size_t length);
void responseSetHeaderLength(Response *response, size_t length);
void responseSetContentLength(Response *response, size_t length);
char* findMimeType(char *filename);
void freeResponse(Response *response);

Response *responseIndex(char *dirPath);
Response *response403(char *filepath);
Response *responseStaticFile(char *filepath);

#endif