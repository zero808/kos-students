#include <stdlib.h>
#include <stdio.h>

#include <kos_client.h>
#include <hashtable.h>
#include <buffer.h>

hashtable **shards;
buffer *b;
sem_t sem_client;
sem_t sem_server;

int kos_init(int num_server_threads, int buf_size, int num_shards) {

    int ix;
    b = init_buffer(buf_size);

    for(ix = 0; ix < num_shards; ix += 1) {
        shards[ix] = init_hashtable(HT_SIZE);
    }

    return 0;
}

char* kos_get(int clientid, int shardId, char* key) {

    return NULL;
}


char* kos_put(int clientid, int shardId, char* key, char* value) {

    return NULL;
}

char* kos_remove(int clientid, int shardId, char* key) {

    return NULL;
}

KV_t* kos_getAllKeys(int clientid, int shardId, int* dim) {

    return NULL;
}

void *server_thread(void *arg)
{
    int *pos;
    pos = (int*) arg;
    KV_t *pair;

    switch(b[*pos].op) {
        case OP_PUT:
            break;
        case OP_REMOVE:
            break;
        case OP_GET:
            pair = get(shards[b[*pos].num_shard], b[*pos].key);
            if(pair == NULL)
            /* b[*pos] */
                ;
            
            break;
        case OP_GETALL:
            break;
        default:
            break;
    }
}
