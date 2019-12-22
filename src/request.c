#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "request.h"
#include "utility.h"
#include "list.h"
#include "kv.h"

void printRequest(Request *request)
{
    printf("path = %s\n", request->path);
    printf("querystring = \n");
    listForEach(request->qslist, (void (*)(void *))printKV);
}

Method toMethod(char *string)
{
    if (!strncmp(string, "GET", 3)) return GET;
    else if (!strncmp(string, "POST", 4)) return POST;
    else return OTHER;
}

Request *HttpRawPacketToRequest(char *packet)
{
    List *lines = split(packet, "\r\n");
    List *startlineItems = split((listGet(lines, 0))->value, " ");
    List *routeItems = split((listGet(startlineItems, 1))->value, "?");
    
    Method method = toMethod(listGet(startlineItems, 0)->value);
    char *path = (listGet(routeItems, 0))->value;
    char *queryString = listGet(routeItems, 1)->value;

    Request *request = malloc(sizeof(Request));
    request->method = method;
    request->path = malloc(strlen(path) + 1);
    memcpy(request->path, path, strlen(path) + 1);
    request->qslist = queryStringNew(queryString);

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

Request *requestNew(Method method, char *path, char *queryString)
{
    Request *request = malloc(sizeof(Request));
    request->method = method;
    request->path = malloc(strlen(path) + 1);
    memcpy(request->path, path, strlen(path) + 1);
    request->qslist = queryStringNew(queryString);
    return request;
}
