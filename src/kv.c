#include <stdio.h>
#include <string.h>
#include "kv.h"
#include "utility.h"

void printKV(KV *kv)
{
    printf("key = %s, value = %s\n", kv->key, kv->value);
}

KV *kvNew(char *key, char *value)
{
    KV *kv = malloc(sizeof(KV));
    kv->key = malloc(sizeof(char) * strlen(key) + 1);
    kv->value = malloc(sizeof(char) * strlen(value) + 1);
    memcpy(kv->key, key, strlen(key) + 1);
    memcpy(kv->value, value, strlen(value) + 1);
    return kv;
}

void kvFree(KV *kv)
{
    free(kv->key);
    free(kv->value);
    free(kv);
}