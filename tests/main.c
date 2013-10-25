#include <stdlib.h>
#include <stdio.h>
#include <hashtable.h>
#include <list.h>
#include <kos_client.h>

void* add_xpto(void *arg);
void* get_xpto(void *arg);

    hashtable *h = NULL;
    char key[8] = "abcdefg";
    char value[3] = "cd";
int main(int argc, char *argv[])
{

    pthread_t tid_p, tid_t1, tid_t2;
    /* hashtable *h = NULL; */
    /* char key[8] = "abcdefg"; */
    /* char value[3] = "cd"; */
    /* KV_t *par = NULL; */
    h = init_hashtable(HT_SIZE);
    if(pthread_create(&tid_p, NULL, add_xpto, (void*) NULL) != 0) {
         puts("Error creating thread.");
         exit(EXIT_FAILURE);
    }
    if(pthread_join(tid_p, NULL) != 0) {
        puts("Error joining thread.");
        exit(EXIT_FAILURE);
    }
    if(pthread_create(&tid_t1, NULL, get_xpto, (void*) NULL) != 0) {
         puts("Error creating thread.");
         exit(EXIT_FAILURE);
    }
    if(pthread_join(tid_t1, NULL) != 0) {
        puts("Error joining thread.");
        exit(EXIT_FAILURE);
    }
    /* add(h, key, value); */
    /* add(h, "bacdefg", value); */
    /* add(h, "cabdefg", value); */
    /* add(h, "badcefg", value); */
    /* add(h, "bacedfg", value); */
    /* add(h, "bacfedg", value); */
    /* add(h, "bacgefd", value); */
    /* add(h, value, key); */
    /* par = get(h, key); */
    /* if(par != NULL) */
    /*     printf("key: %s, value: %s\n", par->key, par->value); */
    /* else */
    /*     puts("erro"); */
    /* add(h, key, "martins"); */
    /* par = get(h, key); */
    /* if(par != NULL) */
    /*     printf("key: %s, value: %s\n", par->key, par->value); */
    /* else */
    /*     puts("erro"); */
    /* lst_print(h->lists[hash(key)]); */
    lst_print(h->lists[hash(value)]);
    delete_hashtable(h);

    exit(EXIT_SUCCESS);
}

void* add_xpto(void *arg) {
    add(h, key, value);
    return NULL;
}

void* get_xpto(void* arg) {
    int dim;
    int ix;
    KV_t* pairs;
    pairs = getAllKeys(h, &dim);
    printf("%d\n", dim);

    for(ix = 0; ix < dim; ix += 1) {
        printf("key: %s, value: %s\n", pairs[ix].key, pairs[ix].value);
    }
    free(pairs);
    return NULL;
}
