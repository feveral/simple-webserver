#ifndef DICTIONARY_H
#define DICTIONARY_H

typedef struct kv {
    char *key;
    char *value;
} KV;


void printKV(KV *kv);
KV *kvNew(char *key, char *value);
void kvFree(KV *kv);

#endif