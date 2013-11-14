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
    for(ix = 0; ix < size; ix += 1) {
        h->readers[ix] = 0;
        pthread_mutex_init(&(h->mutex_counter[ix]), NULL);
        sem_init(&(h->no_waiting[ix]), 0, 1);
        sem_init(&(h->no_accessing[ix]), 0, HT_SIZE);
    }

    return h;

}

int delete_hashtable(hashtable *h)
{
    if(h != NULL) {
        int size = h->size, ix;
        --size;

        for(ix = 0; ix < h->size; ix += 1) {
            pthread_mutex_destroy(&(h->mutex_counter[ix]));
            sem_destroy(&(h->no_waiting[ix]));
            sem_destroy(&(h->no_accessing[ix]));
        }
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

char *get(hashtable *h, char *key) 
{
    int bucket, prev, current;
    char* value = NULL;
    char* ret = NULL;
    if((h != NULL) && (key != NULL)) {
        bucket = hash(key);
        /* queue for accessing */ sem_wait(&(h->no_waiting[bucket]));
        /* lock the mutex in order to access the counter */
        pthread_mutex_lock(&(h->mutex_counter[bucket]));
        prev = h->readers[bucket];
        h->readers[bucket] += 1;
        pthread_mutex_unlock(&(h->mutex_counter[bucket]));
        /* if there were no readers */
        if(prev == 0) {
            /* prevent writes while there is someone already reading or
             * writing */
            sem_wait(&(h->no_accessing[bucket]));
        }
        sem_post(&(h->no_waiting[bucket]));
        value = lst_get(h->lists[bucket], key);
        pthread_mutex_lock(&(h->mutex_counter[bucket]));
        h->readers[bucket] -= 1;
        current = h->readers[bucket];
        pthread_mutex_unlock(&(h->mutex_counter[bucket]));
        if(current == 0) {
            sem_post(&(h->no_accessing[bucket]));
        }
        if(value != NULL) {
            ret = calloc((size_t)(KV_SIZE), sizeof(char));
            if(ret==NULL) {
                fprintf(stderr, "Dynamic memory allocation failed\n");
                exit(EXIT_FAILURE);
            }
            strncpy(ret, value, KV_SIZE);
        }
        return ret;
    }
    else {
        puts("error: received NULL pointer");
        return NULL;
    }
    /* it will never get here, but at least the compiler won't complain */
    return NULL;
}

char *ht_remove(hashtable *h, char *key)
{
    int bucket;
    char *ret;
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
        return NULL;
    }
    /* it will never get here, but at least the compiler won't complain */
    return NULL;
}

char *add(hashtable *h, char *key, char *value)
{
    /* conta como escritor */
    int bucket;
    char* ret = NULL;
    if((h != NULL) && (key != NULL)) {
        bucket = hash(key);
        printf("%d\n", bucket);
        sem_wait(&(h->no_waiting[bucket]));
        printf("%d\n", bucket);
        sem_wait(&(h->no_accessing[bucket]));
        sem_post(&(h->no_waiting[bucket]));
        ret = lst_insert(h->lists[bucket], key, value);
        sem_post(&(h->no_accessing[bucket]));

    }
    else
        /* puts("error: received NULL pointer"); */
        return ret;

    return ret;
}

KV_t* getAllKeys(hashtable *h, int* dim)
{
    if(h != NULL) {
        KV_t* pairs = NULL;
        lst_iitem_t *lst_it = NULL;
        int counter, ix;
        /* we check the number of elements of the lists to
        * avoid using realloc */
        for(ix = 0, counter = 0; ix < h->size; ix += 1) {
            counter += lst_size(h->lists[ix]);
        }
        pairs = calloc((size_t) counter, sizeof(KV_t));
        if(pairs==NULL) {
            fprintf(stderr, "Dynamic memory allocation failed\n");
            exit(EXIT_FAILURE);
        }

        /* for every list in the hashtable */
        for(ix = 0; ix < h->size; ix += 1) {

            /* for every element in the list */
            for(lst_it = h->lists[ix]->first; lst_it != NULL; lst_it = lst_it->next) {
                strncpy(pairs[ix].key, lst_it->item->key, KV_SIZE);
                strncpy(pairs[ix].value, lst_it->item->value, KV_SIZE);
            }
        }
        *dim = counter;
        return pairs;
    }
    return NULL;

}

