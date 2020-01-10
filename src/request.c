#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>

#include "request.h"
#include "utility.h"
#include "list.h"
#include "kv.h"

void printRequest(Request *request)
{
    changePrintColor("bold-green");
    printf("[Request]\n");
    changePrintColor("white");
    printf(" - method = %s\n", methodToString(request->method));
    printf(" - path = %s\n", request->path);
    printf(" - querystring = ");
    changePrintColor("red");
    ListCell *current = request->qslist->head;
    while(current != NULL) {
        KV *kv = current->value;
        printf("<%s, %s> ", kv->key, kv->value);
        current = current->next;
    }
    changePrintColor("white");
    printf("\n - headers = \n");
    changePrintColor("red");
    current = request->headers->head;
    while(current != NULL) {
        KV *kv = current->value;
        printf("    <%s, %s> \n", kv->key, kv->value);
        current = current->next;
    }
    changePrintColor("bold-green");
    printf("[End Of Request]\n");
    changePrintColor("white");
}

Method toMethod(char *string)
{
    if (!strncmp(string, "GET", 3)) return GET;
    else if (!strncmp(string, "POST", 4)) return POST;
    else return OTHER;
}

char *methodToString(Method method)
{
    if (method == GET) return "GET";
    else if (method == POST) return "POST";
    else return "OTHER";
}

Request *requestNew(char *packet, struct sockaddr_in *sin)
{
    List *lines = split(packet, "\r\n");
    List *startlineItems = split((listGet(lines, 0)), " ");
    List *routeItems = split((listGet(startlineItems, 1)), "?");
    
    Method method = toMethod(listGet(startlineItems, 0));
    char *uri = (listGet(startlineItems, 1));
    char *path = (listGet(routeItems, 0));
    char *queryString;
    if (routeItems->count == 1) queryString = "";
    else queryString = listGet(routeItems, 1);

    Request *request = malloc(sizeof(Request));
    request->clientIP = inet_ntoa(sin->sin_addr);
    request->clientPort = ntohs(sin->sin_port);
    request->method = method;

    request->uri = malloc(strlen(uri) + 1);
    request->path = malloc(strlen(path) + 1);
    request->queryString = malloc(strlen(queryString) + 1);
    memcpy(request->path, path, strlen(path) + 1);
    memcpy(request->queryString, queryString, strlen(queryString) + 1);
    memcpy(request->uri, uri, strlen(uri) + 1);
    request->qslist = queryListNew(queryString);
    request->headers = headerListNew(packet);
    request->body = (strstr(packet, "\r\n\r\n")) + 4;

    listFree(lines);
    listFree(startlineItems);
    listFree(routeItems);

    return request;
}

List *queryListNew(char *queryString)
{
    if (strlen(queryString) == 0) return listNew();
    List *splitItems = split(queryString, "&");
    List *qslist = listNew();
    ListCell *current = splitItems->head;
    while(current != NULL) {
        List *keyandvalue = split(current->value, "=");
        KV *kv = kvNew((listGet(keyandvalue, 0)), (listGet(keyandvalue, 1)));
        listAppend(qslist, kv, sizeof(KV));
        current = current->next;
    }
    return qslist;
}

List *headerListNew(char *packet)
{
    List *headers = listNew();
    List *lines = split(packet, "\r\n");
    for(int i = 1; i< lines->count-2; i++) {
        char *key, *value;
        int firstColonIndex = 0;
        char *headerline = listGet(lines, i);
        for(int j = 0; j < strlen(headerline); j++) {
            if (headerline[j] == ':') {
                firstColonIndex = j;
                break;
            }
        }
        KV *kv = kvNew(subString(headerline, 0, firstColonIndex), 
                        subString(headerline, firstColonIndex+2, strlen(headerline)));
        listAppend(headers, kv, sizeof(KV));
    }
    return headers;
}

char *requestGetHeader(Request *request, char *name)
{
    List *headers = request->headers;
    ListCell *current = headers->head;
    while(current != NULL) {
        KV *kv = current->value;
        if (!strncmp(kv->key, name, strlen(kv->key))) {
            return kv->value;
        }
        current = current->next;
    }
    return "";
}

void freeRequest(Request *request)
{
    free(request->path);
    free(request->qslist);
    free(request);
}
