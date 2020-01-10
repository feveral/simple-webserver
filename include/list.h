#ifndef LIST_H
#define LIST_H

#include <stdlib.h>

typedef struct listCell {
    void *value;
    struct listCell *next; 
    size_t size;
} ListCell;

typedef struct list {
    ListCell *head;
    ListCell *tail;
    int count;
} List;

List* listNew();
ListCell *listCellNew(void *value, size_t size);
void listAppend(List *list, void *value, size_t size);
void listAppendString(List *list, char *string);
void listForEach(List *list, void (*func)(void *));
void* listGet(List *list, int index);
void listFree(List *list);

#endif