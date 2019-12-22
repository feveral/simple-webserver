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
#include "list.h"

int main(int argc, char *argv[])
{
    Server *server = serverNew("./", argv[1]);
    serverServe(server);
    return 0;
}