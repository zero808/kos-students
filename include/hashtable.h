#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <kos_client.h>
#include <list.h>
#define HT_SIZE 10

typedef struct hashtable {
    int size;
    list_t **lists;
} hashtable;

hashtable* init_hashtable(int size);
int delete_hashtable(hashtable *h);
int hash(char *key);
KV_t *get(hashtable *h, char *key);
int ht_remove(hashtable *h, char *key);
void add(hashtable *h, char *key, char *value);

#endif
