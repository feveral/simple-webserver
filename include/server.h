#ifndef SERVER_H
#define SERVER_H

#include "list.h"
#include "response.h"
#include "request.h"

typedef struct server {
    int fd;
    char *path;
    char *port;
    List *handlers;
} Server;


Server *serverNew(char *path, char *port);
void serverServe(Server *server);
void serverUse(Server *server, Response *(*func)(Request *));

#endif