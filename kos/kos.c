#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <kos_client.h>
#include <buffer.h>
#include <hashtable.h>
#include <delay.h>

#define LINESIZE 41

hashtable **shards = NULL;
buffer *b = NULL;
pthread_t* threads = NULL;
/* this tracks the line where we're writing on the file */
int *file_pos = NULL;
/* when we remove an element from the kos we save its position in this list so
 * we can overwrite that line with the data of a new element. This way we avoid
 * searching the whole file for the data of the element that was removed and delete it.*/
list_t **invalids = NULL;
FILE ** files; /* the fshardId */
pthread_mutex_t *mutexFiles;

/* Variables and functions needed for the producers-consumers problem */
int index_producer = 0, index_consumer = 0;
pthread_mutex_t mutex, mutex_filepos;
sem_t semCanProd, semCanCons;

/* Function Prototypes */
void op_handler(item *i);
void producer(item *ip);
void consumer();
void populate(int shardId, char*key, char*value);
void *server_thread(void *arg);
void writeToFile(int shardID, char* key, char* value, int position);
void populate(int shardId, char* key, char* value);

int kos_init(int num_server_threads, int buf_size, int num_shards)
{

    int ix = 0;
    FILE *f;
    char name[KV_SIZE];
    /* used in strtok */
    char delim[2] = " ";
    char key_file[KV_SIZE];
    char value_file[KV_SIZE];
    char* s;
    char line[LINESIZE];

    b = init_buffer(buf_size);
    sem_init(&semCanProd, 0, buf_size);
    sem_init(&semCanCons, 0, 0);
    pthread_mutex_init(&mutex, NULL);
    pthread_mutex_init(&mutex_filepos, NULL);
    threads = calloc(num_server_threads, sizeof(pthread_t));
    mutexFiles = calloc(num_shards, sizeof(pthread_mutex_t));

    /* Alloc memory for the pointers to the hashtables */
    shards = calloc(num_shards, sizeof(hashtable*));
    if(shards==NULL) {
        fprintf(stderr, "Dynamic memory allocation failed\n");
        exit(EXIT_FAILURE);
    }

    /* Alloc memory for the pointers to the lists */
    invalids = calloc(num_shards, sizeof(list_t*));
    if(invalids==NULL) {
        fprintf(stderr, "\ndynamic memory allocation failed\n");
        exit(EXIT_FAILURE);
    }

    /* Alloc memory for the hashtables and list */
    for(ix = 0; ix < num_shards; ix += 1) {
        shards[ix] = init_hashtable(HT_SIZE);
        invalids[ix] = lst_new();
        pthread_mutex_init(&mutexFiles[ix], NULL);
    }

    file_pos = calloc(num_shards, sizeof(int));
    if(file_pos==NULL) {
        fprintf(stderr, "\ndynamic memory allocation failed\n");
        exit(EXIT_FAILURE);
    }

    for(ix = 0; ix < num_server_threads; ix += 1) {
        if(pthread_create(&threads[ix], NULL, &server_thread, NULL) != 0) {
            fprintf(stderr, "Threads creation failed\n");
            exit(EXIT_FAILURE);
        }
    }

    files = calloc(num_shards, sizeof(FILE*));
    if(files==NULL) {
        fprintf(stderr, "\ndynamic memory allocation failed\n");
        exit(EXIT_FAILURE);
    }

    /* This is done to create the files if they don't exist since
     * execlp is not working, fuck knows why */
    for(ix = 0; ix < num_shards; ++ix) {

        snprintf(name, KV_SIZE, "fshard%d", ix);

        if((files[ix] = fopen(name, "r")) != NULL) {
            /* LINESIZE = (KV_SIZE -1) * 2 + 1espaço + 1\n */
            while(fgets(line, LINESIZE, files[ix]) != NULL) {
                /* get the line from the file and get two tokens, the key and
                 * its respective value */
                s = strtok(line,delim);
                strncpy(key_file, s, KV_SIZE);
                s = strtok(NULL, delim);
                strncpy(value_file, s, KV_SIZE);
                populate(ix ,key_file, value_file);
            }
        }
        else {
            files[ix] = fopen(name, "w");
            if(files[ix] == NULL) {
                fprintf ( stderr, "couldn't open file '%s'; %s\n",
                        name, strerror(errno));
                exit(EXIT_FAILURE);
            }
            if(fclose(files[ix]) == EOF ) {
                fprintf ( stderr, "couldn't close file '%s'; %s\n",
                        name, strerror(errno));
                exit(EXIT_FAILURE);
            }
            files[ix] = fopen(name, "r+");
            if(files[ix] == NULL) {
                fprintf ( stderr, "couldn't open file '%s'; %s\n",
                        name, strerror(errno));
                exit(EXIT_FAILURE);
            }
        }
    }

    return 0;
}

/* This is similar to the kos_put function except that we don't care about
 * concurrency since this runs before any client can make any request */
void populate(int shardId, char* key, char* value)
{
    int file_position = -1;
    file_position = file_pos[shardId];
    file_pos[shardId] += 1;
    add(shards[shardId], key, value, file_position);
}

char* kos_get(int clientid, int shardId, char* key)
{
    char *ret = NULL;
    item *i = NULL;
    i = init_item();

    write_item(i, clientid, shardId, OP_GET, key, NULL, NULL, DONOTCHANGE);
    producer(i);
    if(i->value != NULL) {
        ret = calloc(KV_SIZE, sizeof(char));
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
    int file_position = -1;
    char *ret = NULL;
    item *i = NULL;
    i = init_item();

    /* get the correct position to write on the file */
    pthread_mutex_lock(&mutex_filepos);
    if(lst_size(invalids[shardId]) == 0) {
        file_position = file_pos[shardId];
        file_pos[shardId] += 1;
    }
    else {
        file_position = lst_remove_pos(invalids[shardId]);
    }
    pthread_mutex_unlock(&mutex_filepos);
    write_item(i, clientid, shardId, OP_PUT, key, value, NULL, file_position);
    producer(i);
    if(i->value != NULL) {
        ret = calloc(KV_SIZE, sizeof(char));
        if(ret==NULL) {
            fprintf(stderr, "Dynamic memory allocation failed\n");
            exit(EXIT_FAILURE);
        }
        strncpy(ret, i->value, KV_SIZE);
    }
    if(ret != NULL) {
        writeToFile(shardId, key, value, i->file_position);
        /* we incremented the counter but we haven't used it */
        lst_insert_pos(invalids[shardId], file_position);
    }
    else {
        writeToFile(shardId, key, value, file_position);
    }
    destroy_item(i);
    return ret;
}

char* kos_remove(int clientid, int shardId, char* key)
{
    char *ret = NULL;
    item *i = NULL;
    i = init_item();

    write_item(i, clientid, shardId, OP_REMOVE, key, NULL, NULL, DONOTCHANGE);
    producer(i);
    ret = i->value;
    i->value = NULL;
    lst_insert_pos(invalids[shardId], i->file_position);
    destroy_item(i);
    return ret;
}

KV_t* kos_getAllKeys(int clientid, int shardId, int* dim)
{
    item *i = NULL;
    KV_t *pair;
    i = init_item();

    write_item(i, clientid, shardId, OP_GETALL, NULL, NULL, NULL, DONOTCHANGE);
    producer(i);
    *dim = i->dimension;
    pair = i->pair;
    /* we want to have a pointer to that pair */
    i->pair = NULL;
    destroy_item(i);
    return pair;
}

void writeToFile(int shardId, char* key, char* value, int position)
{
    char name[KV_SIZE], key_copy[KV_SIZE], value_copy[KV_SIZE];
    int ix, spaces;
    snprintf(name, KV_SIZE, "fshard%d", shardId);
    strncpy(key_copy, key, KV_SIZE);
    strncpy(value_copy, value, KV_SIZE);

    /* fill the strings with spaces in order to be easier to read from the file
     * since we always know how many chars there are */
    spaces = KV_SIZE - 1;
    for(ix = strlen(key); ix < spaces; ++ix) {
        key_copy[ix] = ' ';
    }
    key_copy[ix] = '\0';

    for(ix = strlen(value); ix < spaces; ++ix) {
        value_copy[ix] = ' ';
    }
    value_copy[ix] = '\0';

    /* we want to check if we have to back and how many lines */
    pthread_mutex_lock(&mutexFiles[shardId]);
    if(position != DONOTCHANGE) {
        /* 40 = (KV_SIZE-1) * 2 + ' ' + '\n') */
        fseek(files[shardId], 40 * sizeof(char) * position, SEEK_SET);
    }
    fprintf(files[shardId], "%s %s\n", key_copy, value_copy);
    fflush(files[shardId]);
    pthread_mutex_unlock(&mutexFiles[shardId]);
}

/* Producer-Consumer problem - page 239, 1st edition */
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
    lst_ret_t *oldvalue = NULL;
    KV_t *pair = NULL;
    delay();
        switch(i->op) {
            case OP_PUT:
                oldvalue = add(shards[i->shardID], i->key, i->value, i->file_position);
                write_item(i, DONOTCHANGE, DONOTCHANGE, DONOTCHANGE, NULL, oldvalue->value, NULL, oldvalue->position);
                break;
            case OP_REMOVE:
                oldvalue = ht_remove(shards[i->shardID], i->key);
                if(oldvalue == NULL) {
                    write_item(i, DONOTCHANGE, DONOTCHANGE, DONOTCHANGE, NULL, NULL, NULL, DONOTCHANGE);
                }
                else {
                    write_item(i, DONOTCHANGE, DONOTCHANGE, DONOTCHANGE, NULL, oldvalue->value, NULL, oldvalue->position);
                }
                break;
            case OP_GET:
                oldvalue = get(shards[i->shardID], i->key);
                if(oldvalue == NULL) {
                    write_item(i, DONOTCHANGE, DONOTCHANGE, DONOTCHANGE, NULL, NULL, NULL, DONOTCHANGE);
                }
                else {
                    write_item(i, DONOTCHANGE, DONOTCHANGE, DONOTCHANGE, NULL, oldvalue->value, NULL, oldvalue->position);
                }
                break;
            case OP_GETALL:
                pair = getAllKeys(shards[i->shardID], &(i->dimension));
                write_item(i, DONOTCHANGE, DONOTCHANGE, DONOTCHANGE, NULL, NULL, pair, DONOTCHANGE);
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
    delay();
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
        consumer();
    }
}

