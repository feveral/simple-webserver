#include <string.h>
#include "list.h"

List* listNew()
{
    List *list = malloc(sizeof(List));
    list->head = NULL;
    list->tail = NULL;
    list->count = 0;
    return list;
}

ListCell *listCellNew(void *value, size_t size)
{
    ListCell *cell = malloc(sizeof(ListCell));
    cell->value = malloc(size);
    cell->size = size;
    memcpy(cell->value, value, size);
    cell->next = NULL;
    return cell;
}

void listAppend(List *list, void *value, size_t size)
{
    ListCell *cell = listCellNew(value, size);
    list->count ++;
    if (list->head == NULL && list->tail == NULL) {
        list->head = cell;
        list->tail = cell;
    } else {
        list->tail->next = cell;
        list->tail = cell;
        cell->next = NULL;
    }
}

void listAppendString(List *list, char *string)
{
    ListCell *cell = listCellNew(string, sizeof(char) * (strlen(string) + 1));
    list->count ++;
    if (list->head == NULL && list->tail == NULL) {
        list->head = cell;
        list->tail = cell;
    } else {
        list->tail->next = cell;
        list->tail = cell;
        cell->next = NULL;
    }
}

void listForEach(List *list, void (*func)(void *))
{
    ListCell *current = list->head;
    while(current != NULL) {
        func(current->value);
        current = current->next;
    }
}

void *listGet(List *list, int index)
{
    ListCell *current = list->head;
    if (index < list->count) {
        for (int i = 0; i < index; i++) {
            current = current->next;
        }
        return current->value;
    }
    return NULL;
}

void listFree(List *list)
{
    ListCell *current = list->head;
    while(current != NULL) {
        ListCell *tmp = current;
        current = current->next;
        free(tmp->value);
        free(tmp);
    }
    free(list);
}