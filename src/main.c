#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "server.h"
#include "response.h"
#include "utility.h"
#include "handler.h"
#include "list.h"

int main(int argc, char *argv[])
{
    char *port = "80";
    char *path = "./";
    if (argc == 1 || argc > 3) { printString("usage: ./webserver [port] [path]"); exit(0); }
    if (argc >= 2) port = argv[1];
    if (argc == 3) path = argv[2];
    Server *server = serverNew(path, port);
    serverUse(server, staticHandler);
    serverUse(server, directoryHandler);
    serverServe(server);
    return 0;
}