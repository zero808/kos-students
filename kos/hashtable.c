#include <hashtable.h>
#include <stdlib.h>
#include <stdio.h>
/* typedef struct hashtable {
 *     int size;
 *     list_t *lists[9];
 * } hashtable;
 */
/* typedef struct lst_iitem {
 *    KV_t *item;
 *    struct lst_iitem *next;
 * } lst_iitem_t;
 */

/* list_t */
/* typedef struct {
 *    lst_iitem_t * first;
 * } list_t;
 */

int init_hashtable(hashtable *h, int size)
{
    int ix;

    /* allocate memory for the table */
    h = calloc(1, sizeof(hashtable));
    if(h==NULL) {
        fprintf(stderr, "Dynamic memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    h->size = size;
    printf("size: %d\n", h->size);

    /* FIXME: Consider moving this code to a new function
     * in list.c so we can call it without having to worry
     * how a list is implemented */

    /* allocate memory for the buckets */
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

    return 0;

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

        /* free the pointer */
        free(h);
        h = NULL;
    }
    return 0;
}

KV_t *get(hashtable *h, char *key, int shard)
{
    if(h != NULL)
        return lst_get(h->lists[shard], key);
    else {
        puts("error: received NULL pointer");
        return NULL;
    }
}

int ht_remove(hashtable *h, char *key,  int shard)
{
    if(h != NULL)
        return lst_remove(h->lists[shard], key);
    else {
        puts("error: received NULL pointer");
        return -1;
    }
}

void add(hashtable *h, char *key, char *value, int shard)
{
    if(h != NULL)
        lst_insert(h->lists[shard], key, value);
    else
        puts("error: received NULL pointer");
}

