#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <kos_client.h>
#include <list.h>
#include <semaphore.h>
#include <pthread.h>
#define HT_SIZE 10

typedef struct hashtable {
    /* number of elements */
    int size;
    /* number of readers */
    int readers[HT_SIZE];
    /* pointers to the lists */
    list_t **lists;
    /* only one writer can write in a given list at a time but
     * there can be several writers writing in different lists */
    pthread_mutex_t mutex_writers;
    /* mutexes for accessing the number of readers of each list */
    pthread_mutex_t mutex_counter[HT_SIZE];
    /* Number of threads queued */
    sem_t no_waiting[HT_SIZE];
    /* Number of threads accessing */
    sem_t no_accessing[HT_SIZE];
} hashtable;

hashtable* init_hashtable(int size);
int delete_hashtable(hashtable *h);
int hash(char *key);
KV_t *get(hashtable *h, char *key);
int ht_remove(hashtable *h, char *key);
char* add(hashtable *h, char *key, char *value);
KV_t* getAllKeys(hashtable *h, int *dim);

#endif
