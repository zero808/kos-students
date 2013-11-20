#include <buffer.h>
/* typedef struct _buffer { */
/*     int clientID; */
/*     int num_shard; */
/*     /1* for kos_getAllKeys *1/ */
/*     int dimension; */
/*  int op;  */
/*     char value[KV_SIZE]; */
/*     char key[KV_SIZE]; */
/* } buffer; */
int buffer_size;
buffer* init_buffer(int size)
{
    int ix;
    buffer *b = NULL;
    buffer_size = size;

    b = calloc((size_t) size, sizeof(buffer));
    if(b==NULL) {
        fprintf(stderr, "Dynamic memory allocation failed\n");
        exit(EXIT_FAILURE);
    }

    for(ix = 0; ix < size; ix += 1) {
        sem_init(&(b[ix].waiting), 0, 1);
        sem_wait(&(b[ix].waiting));
        pthread_mutex_init(&(b[ix].buffer_mutex), NULL);
        b[ix].pair = NULL;
        b[ix].modified = 0;
    }

    return b;
}

int write_buffer(buffer *b, int pos, int clientID, int shardID, int op, char *key, char *value, KV_t *pair, int id)
{
    pthread_mutex_lock(&(b[pos].buffer_mutex));
    if (clientID != -1) {
        b[pos].clientID = clientID;
    }
    if (shardID != -1) {
        b[pos].num_shard = shardID;
    }
    if (op != -1) {
        b[pos].op = op;
    }

    if (op == OP_PUT && id == CLIENT) {
        strncpy(b[pos].value, value, KV_SIZE);
    }

    if (op == OP_GET && id == SERVER) {
        if (value == NULL) {
            b[pos].modified = 0;
        }
        else {
            strncpy(b[pos].value, value, KV_SIZE);
            b[pos].modified = 1;
        }
    }

    if (op == OP_PUT && id == SERVER) {
        if (value == NULL) {
            b[pos].modified = 0;
        }
        else {
            strncpy(b[pos].value, value, KV_SIZE);
            b[pos].modified = 1;
        }
    }

    if (op == OP_REMOVE && id == SERVER) {
        if (value == NULL) {
            b[pos].modified = 0;
        }
        else {
            strncpy(b[pos].value, value, KV_SIZE);
            b[pos].modified = 1;
        }
    }
    /* onde anda o get_allkeys? */

    if (key != NULL) {
        strncpy(b[pos].key, key, KV_SIZE);
        /* free(key); */
        /* key = NULL; */
    }
    pthread_mutex_unlock(&(b[pos].buffer_mutex));
    return 0;
}
/* int read_buffer(buffer *b, int pos, int clientID, int shardID) */
/* { */
/*     b[pos].op = OP_GET; */
/* } */
void destroy_buffer(buffer *b)
{
    int ix;

    for(ix = 0; ix < buffer_size; ix += 1) {
        pthread_mutex_destroy(&(b[ix].buffer_mutex));
        sem_destroy(&(b[ix].waiting));
    }
    free(b);
}
