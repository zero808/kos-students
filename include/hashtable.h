#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <kos_client.h>
#include <list.h>

typedef struct hashtable {
    int size;
    /* list_t *lists[2]; */
    list_t **lists;
} hashtable;

hashtable* init_hashtable(int size);
int delete_hashtable(hashtable *h);
KV_t *get(hashtable *h, char *key, int shard);
int ht_remove(hashtable *h, char *key,  int shard);
void add(hashtable *h, char *key, char *value, int shard);

#endif
