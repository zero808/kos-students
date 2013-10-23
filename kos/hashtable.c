#include <hashtable.h>
#include <stdlib.h>
#include <stdio.h>

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
    printf("size: %d\n", h->size);


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
    int bucket;

    if((h != NULL) && (key != NULL)) {
        bucket = hash(key);
        if(bucket != -1) {
            return lst_get(h->lists[bucket], key);
        }
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
    int bucket;
    if((h != NULL) && (key != NULL)) {
        bucket = hash(key);
        if(bucket != -1) {
            return lst_remove(h->lists[bucket], key);
        }
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
    int bucket;
    if((h != NULL) && (key != NULL)) {
        bucket = hash(key);
        if(bucket != -1) {
            lst_insert(h->lists[bucket], key, value);
        }
    }
    else
        puts("error: received NULL pointer");
}


