#ifndef  BUFFER_H
#define  BUFFER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <kos_client.h>

#define OP_PUT 1
#define OP_GET 2
#define OP_REMOVE 3
#define OP_GETALL 4

typedef struct _buffer {
    int clientID;
    int num_shard;
    /* for kos_getAllKeys */
    int dimension;
    int op;
    char value[KV_SIZE];
    char key[KV_SIZE];
} buffer;

buffer* init_buffer(int size);
int write_buffer(buffer *b, int pos, int clientID, int shardID, char *key, char *value);
int read_buffer(buffer *b, int pos, int clientID, int shardID);
int destroy_buffer(buffer *b);
#endif
