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
#define DONOTCHANGE -1


typedef struct _item {
    /* The client ID */
    int clientID;
    /* the number of the shard to use */
    int shardID;
    /* for kos_getAllKeys */
    int dimension;
    /* the code of the operation */
    int op;
    int file_position;
    char key[KV_SIZE];
    char *value;
    KV_t *pair;
    /* to wait for the server to put the answer on the buffer */
    sem_t waiting;
} item;

typedef struct _buffer {
    int size;
    /* instead of the client putting the request on the buffer and
     * waiting for the server to reply, we just put there the address
     * of an item and the server just writes to that adress */
    item **items;
} buffer;

item* init_item();
void destroy_item(item *i);
buffer* init_buffer(int size);
void destroy_buffer(buffer *b);
void write_item(item *i, int clientID, int shardID, int op, char *key, char *value, KV_t *pair, int file_position);
#endif
