#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <kos_client.h>
#include <list.h>
#include <semaphore.h>
#include <pthread.h>
#define HT_SIZE 10
#define FALSE 0
#define TRUE 1

typedef struct hashtable {
    /* number of elements */
    int size;
    /* pointers to the lists */
    list_t **lists;
    /* read-write lock */
    pthread_rwlock_t rwlock[HT_SIZE];
} hashtable;

hashtable* init_hashtable(int size);
int delete_hashtable(hashtable *h);
int hash(char *key);
lst_ret_t *get(hashtable *h, char *key);
lst_ret_t *ht_remove(hashtable *h, char *key);
lst_ret_t *add(hashtable *h, char *key, char *value, int file_position);
KV_t* getAllKeys(hashtable *h, int *dim);

#endif

