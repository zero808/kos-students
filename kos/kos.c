#include <stdlib.h>
#include <stdio.h>

#include <kos_client.h>
#include <buffer.h>
#include <hashtable.h>
#include <delay.h>

#define SERVER 1
#define CLIENT 0

void *server_thread(void *arg);
hashtable **shards = NULL;
buffer *b = NULL;
sem_t sem_client;
sem_t sem_server;
pthread_mutex_t position_mutex;
unsigned int client_position = 0;
unsigned int server_position = 0;
unsigned int buffer_size = 0;

int kos_init(int num_server_threads, int buf_size, int num_shards)
{

    int ix;
    /* hashtable **ss = NULL; */
    b = init_buffer(buf_size);
    buffer_size = buf_size;
    sem_init(&sem_client, 0, buf_size);
    sem_init(&sem_server, 0, 0);
    pthread_mutex_init(&position_mutex, NULL);
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
    int pos;

    /*delay();*/
    sem_wait(&sem_client);
    pthread_mutex_lock(&position_mutex);
    ++client_position;
    client_position %= buffer_size;
    pos = client_position;
    pthread_mutex_unlock(&position_mutex);
    write_buffer(b, pos, clientid, shardId, OP_GET, key, NULL, NULL,CLIENT);
    sem_post(&sem_server);
    /* pthread_mutex_lock(&(b[pos].waiting)); */
    sem_wait(&(b[pos].waiting));
    /*  LOCK  */
    if(b[pos].modified == 1){

        sem_post(&sem_client);
        return b[pos].value;
    }
    else {

        sem_post(&sem_client);
        return NULL;
    }
}


char* kos_put(int clientid, int shardId, char* key, char* value)
{
    int pos;
    char *ret = NULL;

    /*delay();*/
    sem_wait(&sem_client);
    pthread_mutex_lock(&position_mutex);
    ++client_position;
    client_position %= buffer_size;
    pos = client_position;
    pthread_mutex_unlock(&position_mutex);
    write_buffer(b, pos, clientid, shardId, OP_PUT, key, value, NULL, CLIENT);
    sem_post(&sem_server);
    /* pthread_mutex_lock(&(b[pos].waiting)); */
    sem_wait(&(b[pos].waiting));
    if(b[pos].modified == 1){
        ret = calloc((size_t)(KV_SIZE), sizeof(char));
        if(ret==NULL) {
            fprintf(stderr, "Dynamic memory allocation failed\n");
            exit(EXIT_FAILURE);
        }
        strncpy(ret, b[pos].value, KV_SIZE);

        sem_post(&sem_client);
        return ret;
    }
    else {
        sem_post(&sem_client);
        return NULL;
    }
}

char* kos_remove(int clientid, int shardId, char* key)
{
    int pos;
    char *ret = NULL;

    /*delay();*/
    sem_wait(&sem_client);
    pthread_mutex_lock(&position_mutex);
    ++client_position;
    client_position %= buffer_size;
    pos = client_position;
    pthread_mutex_unlock(&position_mutex);
    write_buffer(b, pos, clientid, shardId, OP_REMOVE, key, NULL, NULL, CLIENT);
    sem_post(&sem_server);
    /* pthread_mutex_lock(&(b[pos].waiting)); */
    sem_wait(&(b[pos].waiting));
    if(b[pos].modified == 1){
        ret = calloc((size_t)(KV_SIZE), sizeof(char));
        if(ret==NULL) {
            fprintf(stderr, "Dynamic memory allocation failed\n");
            exit(EXIT_FAILURE);
        }
        strncpy(ret, b[pos].value, KV_SIZE);

        sem_post(&sem_client);
        return ret;
    }
    else {

        sem_post(&sem_client);
        return NULL;
    }
}

KV_t* kos_getAllKeys(int clientid, int shardId, int* dim)
{
    int pos;

    /*delay();*/
    sem_wait(&sem_client);
    pthread_mutex_lock(&position_mutex);
    ++client_position;
    client_position %= buffer_size;
    pos = client_position;
    pthread_mutex_unlock(&position_mutex);
    write_buffer(b, pos, clientid, shardId, OP_GETALL, NULL, NULL, NULL, CLIENT);
    sem_post(&sem_server);
    /* pthread_mutex_lock(&(b[pos].waiting)); */
    sem_wait(&(b[pos].waiting));
    *dim = b[pos].dimension;
    return b[pos].pair;
}

void *server_thread(void *arg)
{
    int pos;
    char *oldvalue = NULL;
    KV_t *pair = NULL;

    while(1) {
        /*delay();*/
        sem_wait(&sem_server);
        pthread_mutex_lock(&position_mutex);
        /* get the correct positiong of the buffer */
        ++server_position;
        server_position %= buffer_size;
        pos = server_position;
        pthread_mutex_unlock(&position_mutex);

        switch(b[pos].op) {
            /* always free the char* returned by the hashtable's functions! */
            case OP_PUT:
                oldvalue = add(shards[b[pos].num_shard], b[pos].key, b[pos].value);
                write_buffer(b, pos, -1, -1, OP_PUT, NULL, oldvalue, NULL, SERVER);
                free(oldvalue); /* if it's null it won't do anything */
                break;
            case OP_REMOVE:
                oldvalue = ht_remove(shards[b[pos].num_shard], b[pos].key);
                write_buffer(b, pos, -1, -1, OP_REMOVE, NULL, oldvalue, NULL, SERVER);
                free(oldvalue);
                break;
            case OP_GET:
                oldvalue = get(shards[b[pos].num_shard], b[pos].key);
                write_buffer(b, pos, -1, -1, OP_GET, NULL, oldvalue, NULL, SERVER);
                free(oldvalue);
                break;
            case OP_GETALL:
                pair = getAllKeys(shards[b[pos].num_shard], &(b[pos].dimension));
                write_buffer(b, pos, -1, -1, OP_GETALL, NULL, NULL, pair,SERVER);
                free(pair);
                break;
            default:
                break;
        }
        /* pthread_mutex_unlock(&(b[pos].waiting)); */
        sem_post(&(b[pos].waiting));
        /* sem_post(&sem_client); */
    }
}
