#include <hashtable.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>

hashtable *init_hashtable(int size)
{
    int ix;
    hashtable *h;

    /* allocate memory for the table */
    h = calloc(1, sizeof(hashtable));
    if(h==NULL) {
        fprintf(stderr, "Dynamic memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    h->size = size;

    /* allocate memory for the pointers to the lists */
    h->lists = calloc((size_t) size, sizeof(list_t*));
    if(h->lists==NULL) {
        fprintf(stderr, "Dynamic memory allocation failed\n");
        exit(EXIT_FAILURE);
    }

    /* allocate memory for the lists and initialize the pointers
     * to the first element in the list */
    for(ix = 0; ix < size; ++ix) {
        h->lists[ix] = calloc(1, sizeof(list_t));
        if(h->lists[ix]==NULL) {
            fprintf(stderr, "Dynamic memory allocation failed\n");
            exit(EXIT_FAILURE);
        }
        h->lists[ix]->first = NULL;
    }


    pthread_mutex_init(&(h->mutex_writers), NULL);
    sem_init(&(h->teste), 0, 2);
    for(ix = 0; ix < size; ix += 1) {
        h->readers[ix] = 0;
        pthread_mutex_init(&(h->mutex_counter[ix]), NULL);
        sem_init(&(h->no_waiting[ix]), 0, 2);
        sem_init(&(h->no_accessing[ix]), 0, HT_SIZE);
    }

    return h;

}

int delete_hashtable(hashtable *h)
{
    if(h != NULL) {
        int size = h->size;
        --size;
        /* free the memory for each list */
        do {
            lst_destroy(h->lists[size]);
        } while(size--);

        /* free the pointers */
        free(h->lists);
        /* free the hashtable */
        free(h);
        h = NULL;
    }
    return 0;
}

int hash(char* key) {

    int i=0;

    if (key == NULL)
        return -1;

    while (*key != '\0') {
        i+=(int) *key;
        key++;
    }

    i=i % HT_SIZE;

    return i;
}

KV_t *get(hashtable *h, char *key)
{
    int bucket, prev, current;
    KV_t* pair;
    if((h != NULL) && (key != NULL)) {
        bucket = hash(key);
        sem_wait(&(h->no_waiting[bucket]));
        pthread_mutex_lock(&(h->mutex_counter[bucket]));
        prev = h->readers[bucket];
        h->readers[bucket] += 1;
        
        pthread_mutex_unlock(&(h->mutex_counter[bucket]));
        if(prev == 0) {
            sem_wait(&(h->no_accessing[bucket]));
        }
        sem_post(&(h->no_waiting[bucket]));
        pair = lst_get(h->lists[bucket], key);
        pthread_mutex_lock(&(h->mutex_counter[bucket]));
        
        h->readers[bucket] -= 1;
        current = h->readers[bucket];
        pthread_mutex_unlock(&(h->mutex_counter[bucket]));
        if(current == 0) {
            sem_post(&(h->no_accessing[bucket]));
        }
        return pair;
    }
    else {
        puts("error: received NULL pointer");
        return NULL;
    }
    /* it will never get here, but at least the compiler won't complain */
    return NULL;
}

int ht_remove(hashtable *h, char *key)
{
    int bucket, ret;
    /*  se nao houver nem leitores nem escritores */
    if((h != NULL) && (key != NULL)) {
        bucket = hash(key);
        sem_wait(&(h->no_waiting[bucket]));
        sem_wait(&(h->no_accessing[bucket]));
        sem_post(&(h->no_waiting[bucket]));
        ret = lst_remove(h->lists[bucket], key);
        sem_post(&(h->no_accessing[bucket]));
        return ret;
    }
    else {
        puts("error: received NULL pointer");
        return -1;
    }
    /* it will never get here, but at least the compiler won't complain */
    return -1;
}

void add(hashtable *h, char *key, char *value)
{
    /* conta como escritor */
    int bucket;
    if((h != NULL) && (key != NULL)) {
    /* pthread_mutex_lock(&(h->mutex_writers)); */
    puts("teste");
    sem_wait(&(h->teste));
    if(errno == EAGAIN)
        puts("nao deu");
        bucket = hash(key);
        printf("%d\n", bucket);
        sem_wait(&(h->no_waiting[bucket]));
        printf("%d\n", bucket);
        sem_wait(&(h->no_accessing[bucket]));
        sem_post(&(h->no_waiting[bucket]));
        lst_insert(h->lists[bucket], key, value);
        sem_post(&(h->no_accessing[bucket]));

    }
    else
        puts("error: received NULL pointer");
}

/* TODO:
 * getALL
 * fix retorno do add, ver enunciado pag 8*/
