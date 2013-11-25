#include <stdlib.h>
#include <stdio.h>

#include <kos_client.h>
#include <buffer.h>
#include <hashtable.h>
#include <delay.h>

hashtable **shards = NULL;
buffer *b = NULL;

/* Variables and functions needed for the producers-consumers problem */
int index_producer = 0, index_consumer = 0;
pthread_mutex_t mutex;
sem_t semCanProd, semCanCons;
void op_handler(item *i);
void producer(item *ip);
void consumer();
void *server_thread(void *arg);

int kos_init(int num_server_threads, int buf_size, int num_shards)
{

    int ix = 0;

    b = init_buffer(buf_size);
    sem_init(&semCanProd, 0, buf_size);
    sem_init(&semCanCons, 0, 0);
    pthread_mutex_init(&mutex, NULL);
    pthread_t* threads=(pthread_t*)calloc(num_server_threads, sizeof(pthread_t));


    shards = calloc((size_t)(num_shards), sizeof(hashtable*));
    if(shards==NULL) {
        fprintf(stderr, "Dynamic memory allocation failed\n");
        exit(EXIT_FAILURE);
    }

    for(ix = 0; ix < num_shards; ix += 1) {
        shards[ix] = init_hashtable(HT_SIZE);
    }
    /* arranjar maneira de no fim libertar a memoria das shards */

    for(ix = 0; ix < num_server_threads; ix += 1) {
        if(pthread_create(&threads[ix], NULL, &server_thread, NULL) != 0) {
            fprintf(stderr, "Threads creation failed\n");
            exit(EXIT_FAILURE);
        }
    }

    /* for(ix = 0; ix < num_server_threads; ix += 1) { */
    /*     if(pthread_join(threads[ix], NULL) != 0) { */
    /*         fprintf(stderr, "Threads cenas failed\n"); */
    /*         exit(EXIT_FAILURE); */
    /*     } */
    /* } */

    return 0;
}

char* kos_get(int clientid, int shardId, char* key)
{
    char *ret = NULL;
    item *i = NULL;
    i = init_item();

    /*delay();*/
    write_item(i, clientid, shardId, OP_GET, key, NULL, NULL);
    producer(i);
    if(i->value != NULL) {
        ret = calloc((size_t)(KV_SIZE), sizeof(char));
        if(ret==NULL) {
            fprintf(stderr, "Dynamic memory allocation failed\n");
            exit(EXIT_FAILURE);
        }
        strncpy(ret, i->value, KV_SIZE);
    }
    destroy_item(i);
    return ret;
}


char* kos_put(int clientid, int shardId, char* key, char* value)
{
    char *ret = NULL;
    item *i = NULL;
    i = init_item();

    /*delay();*/
    write_item(i, clientid, shardId, OP_PUT, key, value, NULL);
    producer(i);
    if(i->value != NULL) {
        ret = calloc((size_t)(KV_SIZE), sizeof(char));
        if(ret==NULL) {
            fprintf(stderr, "Dynamic memory allocation failed\n");
            exit(EXIT_FAILURE);
        }
        strncpy(ret, i->value, KV_SIZE);
    }
    destroy_item(i);
    return ret;
}

char* kos_remove(int clientid, int shardId, char* key)
{
    char *ret = NULL;
    item *i = NULL;
    i = init_item();

    /*delay();*/
    write_item(i, clientid, shardId, OP_REMOVE, key, NULL, NULL);
    producer(i);
    if(i->value != NULL) {
        ret = calloc((size_t)(KV_SIZE), sizeof(char));
        if(ret==NULL) {
            fprintf(stderr, "Dynamic memory allocation failed\n");
            exit(EXIT_FAILURE);
        }
        strncpy(ret, i->value, KV_SIZE);
    }
    destroy_item(i);
    return ret;
}

KV_t* kos_getAllKeys(int clientid, int shardId, int* dim)
{
    item *i = NULL;
    KV_t *pair;
    i = init_item();

    /*delay();*/
    write_item(i, clientid, shardId, OP_GETALL, NULL, NULL, NULL);
    producer(i);
    *dim = i->dimension;
    pair = i->pair;
    destroy_item(i);
    return pair;
}


/* Producer-Consumer problem - page 239, 1st edition */
/* we pass a semaphore because the both client threads and server are
 * producers and consumers afterwards and vice-versa */
/* void producer(int pos_prior, sem_t *semaphore_p, sem_t *semaphore_c) */
/* int  producer(int pos_prior, int clientid, int shardId, int op, char *key, char *value, KV_t *pair) */
void producer(item *ip)
{
    int pos;
    sem_wait(&semCanProd);
    pthread_mutex_lock(&mutex);
    pos = index_producer++;
    index_producer %= b->size;
    b->items[pos] = ip;
    pthread_mutex_unlock(&mutex);
    sem_post(&semCanCons);
    /* lock ourselves until the server processes our request */
    sem_wait(&(ip->waiting));
}

void op_handler(item *i)
{
    char *oldvalue = NULL;
    KV_t *pair = NULL;
        switch(i->op) {
            /* always free the char* returned by the hashtable's functions! */
            case OP_PUT:
                oldvalue = add(shards[i->shardID], i->key, i->value);
                write_item(i, DONOTCHANGE, DONOTCHANGE, DONOTCHANGE, NULL, oldvalue, NULL);
                free(oldvalue); /* if it's null it won't do anything */
                break;
            case OP_REMOVE:
                oldvalue = ht_remove(shards[i->shardID], i->key);
                write_item(i, DONOTCHANGE, DONOTCHANGE, DONOTCHANGE, NULL, oldvalue, NULL);
                free(oldvalue);
                break;
            case OP_GET:
                oldvalue = get(shards[i->shardID], i->key);
                write_item(i, DONOTCHANGE, DONOTCHANGE, DONOTCHANGE, NULL, oldvalue, NULL);
                free(oldvalue);
                break;
            case OP_GETALL:
                pair = getAllKeys(shards[i->shardID], &(i->dimension));
                write_item(i, DONOTCHANGE, DONOTCHANGE, DONOTCHANGE, NULL, NULL, pair);
                free(pair);
                break;
            default:
                break;
        }
}

void consumer()
{
    int pos;
    item *i = NULL;
    sem_wait(&semCanCons);
    pthread_mutex_lock(&mutex);
    pos = index_consumer++;
    index_consumer %= b->size;
    i = b->items[pos];
    b->items[pos] = NULL;
    pthread_mutex_unlock(&mutex);
    sem_post(&semCanProd);
    /* uses  i */
    op_handler(i);
    /* awakes the client */
    sem_post(&(i->waiting));
}

void *server_thread(void *arg)
{

    while(1) {
        /*delay();*/
        consumer();
    }
}

