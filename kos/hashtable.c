#include <hashtable.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>

/* void start_reading(hashtable *h, int bucket); */
/* void finish_reading(hashtable *h, int bucket); */
/* void start_writing(hashtable *h, int bucket); */
/* void finish_writing(hashtable *h, int bucket); */

hashtable *init_hashtable(int size)
{
    int ix;
    hashtable *h = NULL;

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

    for(ix = 0; ix < size; ix += 1) {
        h->readers[ix] = 0;
        h->writing[ix] = FALSE;
        h->readers_waiting[ix] = 0;
        h->writers_waiting[ix] = 0;

        if(pthread_mutex_init(&(h->ht_mutex[ix]), NULL) != 0) {
            fprintf(stderr, "Error initializing ht_mutex[%d], error code is %d\n", ix, errno);
            exit(EXIT_FAILURE);
        }

        if(sem_init(&(h->s_readers[ix]), 0, 0) != 0) {
            fprintf(stderr, "Error initializing s_readers[%d], error code is %d\n", ix, errno);
            exit(EXIT_FAILURE);
        }
        if(sem_init(&(h->s_writers[ix]), 0, 0) != 0) {
            fprintf(stderr, "Error initializing s_writers[%d], error code is %d\n", ix, errno);
            exit(EXIT_FAILURE);
        }
        if(pthread_rwlock_init(&(h->rwlock[ix]), NULL) != 0) {
            fprintf(stderr, "Error initializing s_writers[%d], error code is %d\n", ix, errno);
            exit(EXIT_FAILURE);
        }
    }

    return h;

}

int delete_hashtable(hashtable *h)
{
    if(h != NULL) {
        int size = h->size, ix;
        --size;

        for(ix = 0; ix < h->size; ix += 1) {
            pthread_mutex_destroy(&(h->ht_mutex[ix]));
            sem_destroy(&(h->s_readers[ix]));
            sem_destroy(&(h->s_writers[ix]));
        }
        /* free the memory for each list */
        do {
            lst_destroy(h->lists[size]);
        } while(size--);

        /* free the pointers */
        free(h->lists);
        h->lists = NULL;
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
    int bucket;
    char* value = NULL;

    if((h != NULL) && (key != NULL)) {
        bucket = hash(key);
        /* start_reading(h, bucket); */
        pthread_rwlock_rdlock(&h->rwlock[bucket]);
        value = lst_get(h->lists[bucket], key);
        pthread_rwlock_unlock(&h->rwlock[bucket]);
        /* finish_reading(h, bucket); */
    }
    else {
        puts("error: received NULL pointer");
    }

    return value;
}

char *ht_remove(hashtable *h, char *key)
{
    int bucket;
    char *ret = NULL;

    if((h != NULL) && (key != NULL)) {
        bucket = hash(key);
        /* start_writing(h, bucket); */
        pthread_rwlock_wrlock(&h->rwlock[bucket]);
        ret = lst_remove(h->lists[bucket], key);
        pthread_rwlock_unlock(&h->rwlock[bucket]);
        /* finish_writing(h, bucket); */


    }
    else {
        puts("error: received NULL pointer");
    }
    return ret;
}

char *add(hashtable *h, char *key, char *value)
{
    int bucket;
    char *ret = NULL;

    if((h != NULL) && (key != NULL)) {
        bucket = hash(key);
        /* start_writing(h, bucket); */
        pthread_rwlock_wrlock(&h->rwlock[bucket]);
        ret = lst_insert(h->lists[bucket], key, value);
        pthread_rwlock_unlock(&h->rwlock[bucket]);
        /* finish_writing(h, bucket); */
    }
    else {
        puts("error: received NULL pointer");
    }
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
            pthread_rwlock_rdlock(&h->rwlock[ix]);
            counter += lst_size(h->lists[ix]);
            pthread_rwlock_unlock(&h->rwlock[ix]);
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
                pthread_rwlock_rdlock(&h->rwlock[ix]);
                strncpy(pairs[ix].key, lst_it->item->key, KV_SIZE);
                strncpy(pairs[ix].value, lst_it->item->value, KV_SIZE);
                pthread_rwlock_unlock(&h->rwlock[ix]);
            }
        }
        *dim = counter;
        return pairs;
    }
    return NULL;

}

/* Implementation for the Readers-Writers problem, page 225
 * Use these functions to avoid repeated code */
/* void start_reading(hashtable *h, int bucket) */
/* { */
/*     pthread_mutex_lock(&(h->ht_mutex[bucket])); */
/*     if(h->writing[bucket] || h->writers_waiting[bucket] > 0) { */
/*         ++h->readers_waiting[bucket]; */
/*         pthread_mutex_unlock(&(h->ht_mutex[bucket])); */
/*         sem_wait(&(h->s_readers[bucket])); */
/*         pthread_mutex_lock(&(h->ht_mutex[bucket])); */
/*         if(h->readers_waiting > 0) { */
/*             ++h->readers[bucket]; */
/*             --h->readers_waiting[bucket]; */
/*             sem_post(&(h->s_readers[bucket])); */
/*         } */
/*     } */
/*     else { */
/*         ++h->readers[bucket]; */
/*     } */
/*     pthread_mutex_unlock(&(h->ht_mutex[bucket])); */
/* } */

/* void finish_reading(hashtable *h, int bucket) */
/* { */
/*     pthread_mutex_lock(&(h->ht_mutex[bucket])); */
/*     --h->readers[bucket]; */
/*     if((h->readers[bucket] == 0) && (h->writers_waiting[bucket] > 0)) { */
/*         sem_post(&(h->s_writers[bucket])); */
/*         h->writing[bucket] = TRUE; */
/*         --h->readers_waiting[bucket]; */
/*     } */
/*     pthread_mutex_unlock(&(h->ht_mutex[bucket])); */
/* } */

/* void start_writing(hashtable *h, int bucket) */
/* { */
/*     pthread_mutex_lock(&(h->ht_mutex[bucket])); */
/*     if(h->writing[bucket] || (h->readers[bucket] > 0) || (h->readers_waiting[bucket] > 0)) { */
/*         ++h->writers_waiting[bucket]; */
/*         pthread_mutex_unlock(&(h->ht_mutex[bucket])); */
/*         sem_wait(&(h->s_writers[bucket])); */
/*         pthread_mutex_lock(&(h->ht_mutex[bucket])); */
/*     } */
/*     h->writing[bucket] = TRUE; */
/*     pthread_mutex_unlock(&(h->ht_mutex[bucket])); */
/* } */

/* void finish_writing(hashtable *h, int bucket) */
/* { */
/*     pthread_mutex_lock(&(h->ht_mutex[bucket])); */
/*     h->writing[bucket] = FALSE; */
/*     if(h->readers_waiting[bucket] > 0) { */
/*         sem_post(&(h->s_readers[bucket])); */
/*         ++h->readers[bucket]; */
/*         --h->readers_waiting[bucket]; */
/*     } */
/*     else if(h->writers_waiting[bucket] > 0) { */
/*         sem_post(&(h->s_writers[bucket])); */
/*         h->writing[bucket] = TRUE; */
/*         --h->writers_waiting[bucket]; */
/*     } */
/*     pthread_mutex_unlock(&(h->ht_mutex[bucket])); */
/* } */
