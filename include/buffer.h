#ifndef  BUFFER_H
#define  BUFFER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <kos_client.h>

#define OP_PUT 1
#define OP_GET 2
#define OP_REMOVE 3
#define OP_GETALL 4
#define CLIENT 0
#define SERVER 1


typedef struct _buffer {
    int clientID;
    int num_shard;
    /* for kos_getAllKeys */
    int dimension;
    int op;
    char value[KV_SIZE];
    char key[KV_SIZE];
    KV_t *pair;
    pthread_mutex_t buffer_mutex;
    /* to wait for the server to put the answer on the buffer */
    /* pthread_mutex_t waiting; */
    sem_t waiting;
    /* this is used to know when the string should be null */
    short int modified;
} buffer;

/* TODO
 * Criar:
typedef struct buffer {
    int size;
    item **items;
} buffer;
e renomear _buffer para item para ficar mais óbvio
*/

buffer* init_buffer(int size);
int write_buffer(buffer *b, int pos, int clientID, int shardID, int op, char *key, char *value, KV_t *pair, int id);
/* int read_buffer(buffer *b, int pos, int clientID, int shardID); */
void destroy_buffer(buffer *b);
#endif
