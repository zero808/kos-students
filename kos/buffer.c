#include <buffer.h>

item* init_item()
{
    item* i = NULL;


    i = calloc(1, sizeof(item));
    if(i==NULL) {
        fprintf(stderr, "Dynamic memory allocation failed\n");
        exit(EXIT_FAILURE);
    }

    /* i->value = calloc(KV_SIZE, sizeof(char)); */
    /* if(i->value==NULL) { */
    /*     fprintf(stderr, "Dynamic memory allocation failed\n"); */
    /*     exit(EXIT_FAILURE); */
    /* } */
    i->value = NULL;
    i->pair = NULL;

    /* intialize any semaphores/mutexes if needed */
    sem_init(&(i->waiting), 0, 0);
    return i;
}

void destroy_item(item *i)
{
    if(i != NULL){
        sem_destroy(&(i->waiting));
        if(i->pair != NULL){
            free(i->pair);
        }
        if(i->value != NULL){
            free(i->value);
        }
        free(i);
    }
}

buffer* init_buffer(int size)
{
    buffer *b = NULL;

    b = calloc(1, sizeof(buffer));
    if(b==NULL) {
        fprintf(stderr, "Dynamic memory allocation failed\n");
        exit(EXIT_FAILURE);
    }

    b->size = size;

    b->items = calloc(size, sizeof(item*));
    if(b->items==NULL) {
        fprintf(stderr, "Dynamic memory allocation failed\n");
        exit(EXIT_FAILURE);
    }

    return b;
}

void destroy_buffer(buffer *b)
{
    int ix;

    for(ix = 0; ix < b->size; ix += 1) {
        free(b->items[ix]);
    }
    free(b);
}

/* int write_item(buffer *b, int pos, int clientID, int shardID, int op, char *key, char *value, KV_t *pair, int id) */
void write_item(item *i, int clientID, int shardID, int op, char *key, char *value, KV_t *pair)
{
    if(i != NULL){
        if(clientID != DONOTCHANGE){
            i->clientID = clientID;
        }
        if(shardID != DONOTCHANGE){
            i->shardID = shardID;
        }
        if(op != DONOTCHANGE){
            i->op = op;
        }
        if(key != NULL){
            strncpy(i->key, key, KV_SIZE);
        }
        i->value = value;
        /* until we find something better, we'll always allocate memory for holding the value */
        /* if(value != NULL){ */
        /*     strncpy(i->value, value, KV_SIZE); */
        /* } */
        /* else { */
        /*     if(i->value != NULL){ */
        /*         free(i->value); */
        /*         i->value = NULL; */
        /*     } */
        /* } */
        if(pair != NULL){
            i->pair = pair;
        }
    }
}
