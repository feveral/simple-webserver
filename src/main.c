#include "server.h"
#include "utility.h"
#include "handler.h"

int main(int argc, char *argv[])
{
    char *port = "8080";
    char *path = "./";
    if (argc >= 2) port = argv[1];
    if (argc == 3) path = argv[2];
    if (argc > 3) { printString("usage: ./webserver [port] [path]"); exit(0); }
    Server *server = serverNew(path, port);
    serverUse(server, cgiHandler);
    serverUse(server, staticHandler);
    serverUse(server, directoryHandler);
    serverServe(server);
    return 0;
}