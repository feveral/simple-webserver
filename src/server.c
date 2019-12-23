#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "server.h"
#include "utility.h"
#include "request.h"
#include "response.h"
#include "kv.h"
#include "handler.h"

static void handlePacket(Server *server, int fd, struct sockaddr_in *sin) {
    // printf("[info] connected from %s:%d\n",
    //         inet_ntoa(sin->sin_addr), ntohs(sin->sin_port));

    char reqPacket[20480];
    recv(fd, reqPacket, sizeof(reqPacket), 0);
    Request *request = HttpRawPacketToRequest(reqPacket);
    printRequest(request);

    ListCell *current = (server->handlers)->head;
    Response *response = NULL;
    while(current != NULL) {
        Handler *handler = (current->value);
        response = (*handler)(request);
        current = current->next;
        if (response) break;
    }

    char *resPacket = responsePacket(response);
    size_t packetLength = (response->statusLength) + (response->headerLength) + (response->contentLength);
    printf("packetLength = %d\n", packetLength);
    send(fd, resPacket, packetLength, 0);
    close(fd);

    // printf("[info] disconnected from %s:%d\n",
    //         inet_ntoa(sin->sin_addr), ntohs(sin->sin_port));
}

void serverUse(Server *server, Handler handler)
{
    Handler *hp = malloc(sizeof(Handler));
    *(hp) = handler;
    listAppend(server->handlers, listCellNew(hp, sizeof(Handler)));
}

Server *serverNew(char *path, char *port)
{
    int fd; 
    unsigned val = 1;
    struct sockaddr_in sin;
    fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));

    bzero(&sin, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_port = htons(atoi(port));

    bind(fd, (struct sockaddr*) &sin, sizeof(sin));
    listen(fd, SOMAXCONN);

    Server *server = malloc(sizeof(Server));
    server->path = malloc((sizeof(char))* strlen(path)+1);
    server->port = malloc((sizeof(char))* strlen(port)+1);
    server->handlers = listNew();
    server->fd = fd;

    memcpy(server->path, path, (sizeof(char))* strlen(path)+1);
    memcpy(server->port, path, (sizeof(char))* strlen(port)+1);
    
    return server;
}


void serverServe(Server *server)
{
    pid_t pid;
    int pfd;
    struct sockaddr_in psin;
    while(1) {
        int val = sizeof(psin);
        bzero(&psin, sizeof(psin));
        if((pfd = accept(server->fd, (struct sockaddr*) &psin, &val))<0) perror("accept");
        if((pid = fork()) < 0) perror("fork");
        else if(pid == 0) {
            close(server->fd);
            handlePacket(server, pfd, &psin);
            exit(0);
        }
        close(pfd);
    }
}