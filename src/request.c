#include <stdlib.h>
#include <string.h>
#include <stdio.h>

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
        printf("<%s, %s> ", 
            ((KV *)(current->value))->key,
            ((KV *)(current->value))->value);
        current = current->next;
    }
    changePrintColor("bold-green");
    printf("\n[End Of Request]\n");
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

Request *HttpRawPacketToRequest(char *packet)
{
    List *lines = split(packet, "\r\n");
    List *startlineItems = split((listGet(lines, 0))->value, " ");
    List *routeItems = split((listGet(startlineItems, 1))->value, "?");
    
    Method method = toMethod(listGet(startlineItems, 0)->value);
    char *path = (listGet(routeItems, 0))->value;
    char *queryString;
    if (routeItems->count == 1) queryString = "";
    else queryString = listGet(routeItems, 1)->value;

    Request *request = malloc(sizeof(Request));
    request->method = method;
    request->path = malloc(strlen(path) + 1);
    memcpy(request->path, path, strlen(path) + 1);
    request->qslist = queryStringNew(queryString);

    request->body = (strstr(packet, "\r\n\r\n")) + 4;

    listFree(lines);
    listFree(startlineItems);
    listFree(routeItems);

    return request;
}

List *queryStringNew(char *queryString)
{
    if (strlen(queryString) == 0) return listNew();
    List *splitItems = split(queryString, "&");
    List *qslist = listNew();
    ListCell *current = splitItems->head;
    while(current != NULL) {
        List *keyandvalue = split(current->value, "=");
        KV *kv = kvNew((listGet(keyandvalue, 0))->value, (listGet(keyandvalue, 1))->value);
        listAppend(qslist, listCellNew(kv, sizeof(KV)));
        current = current->next;
    }
    return qslist;
}

// no use function
Request *requestNew(Method method, char *path, char *queryString)
{
    Request *request = malloc(sizeof(Request));
    request->method = method;
    request->path = malloc(strlen(path) + 1);
    memcpy(request->path, path, strlen(path) + 1);
    request->qslist = queryStringNew(queryString);
    return request;
}

void freeRequest(Request *request)
{
    free(request->path);
    free(request->qslist);
    free(request);
}
