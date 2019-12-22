#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "server.h"
#include "utility.h"
#include "list.h"

int main(int argc, char *argv[])
{

    // List *list = split("GET / HTTP/1.1\r\newqjiord\r\nqwjeio\r\n\r\nnnnnnnn", "\r\n");
    // listForEach(list, printString);
    // listFree(list);

    Server *server = serverNew("./", argv[1]);
    serverServe(server);
    return 0;
}