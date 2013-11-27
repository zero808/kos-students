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
    /* number of readers (per list) */
    int readers[HT_SIZE];
    /* has the value 1 if someone is writing, 0 otherwise */
    short int writing[HT_SIZE];
    /* Amount of readers waiting for accessing a given list */
    int readers_waiting[HT_SIZE];
    /* Amount of writers waiting for accessing a given list */
    int writers_waiting[HT_SIZE];
    /* Semaphore for the readers */
    sem_t s_readers[HT_SIZE];
    /* Semaphore for the writers */
    sem_t s_writers[HT_SIZE];
    /* Mutex for accessing shared information of a given list */
    pthread_mutex_t ht_mutex[HT_SIZE];
    pthread_rwlock_t rwlock[HT_SIZE];
} hashtable;

hashtable* init_hashtable(int size);
int delete_hashtable(hashtable *h);
int hash(char *key);
char *get(hashtable *h, char *key);
char *ht_remove(hashtable *h, char *key);
char* add(hashtable *h, char *key, char *value);
KV_t* getAllKeys(hashtable *h, int *dim);

#endif

