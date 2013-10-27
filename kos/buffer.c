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
buffer* init_buffer(int size)
{
    int ix;
    buffer *b = NULL;

    b = calloc((size_t) size, sizeof(buffer));
    if(b==NULL) {
        fprintf(stderr, "Dynamic memory allocation failed\n");
        exit(EXIT_FAILURE);
    }

    return b;
}

int write_buffer(buffer *b, int pos, int clientID, int shardID, char *key, char *value)
{
    /* b[pos].op = OP_PUT; */
    b[pos].clientID = clientID;
    b[pos].num_shard = shardID;
    strncpy(b[pos].key, key, KV_SIZE);
    strncpy(b[pos].value, value, KV_SIZE);
}
/* int read_buffer(buffer *b, int pos, int clientID, int shardID) */
/* { */
/*     b[pos].op = OP_GET; */
/* } */
void destroy_buffer(buffer *b)
{
    int ix;

    free(b);
}
