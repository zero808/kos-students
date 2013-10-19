#include <stdlib.h>
#include <stdio.h>
#include <hashtable.h>
#include <list.h>
#include <kos_client.h>

int main(int argc, char *argv[])
{
    list_t *list;
    KV_t* pair = NULL;
    char oi[3] = "oi", ola[4] = "ola";
    list = lst_new();
    puts("new");
    lst_insert(list, oi, ola);
    puts("insert");
    printf("Tamanho da lista: %d\n", lst_size(list));
    lst_insert(list, ola, oi);
    puts("insert");
    printf("Tamanho da lista: %d\n", lst_size(list));
    lst_print(list);
    pair = lst_get(list, oi);
    if(pair != NULL) {
        printf("key: %s; value: %s\n", pair->key, pair->value);
    }
    else {
        /* the element wasn't found */
        puts("erro");
    }
    printf("%d\n", lst_remove(list, ola));
    puts("remove");
    lst_print(list);
    pair = lst_get(list, oi);
    if(pair != NULL) {
        printf("key: %s; value: %s\n", pair->key, pair->value);
    }
    else {
        puts("erro");
    }
    lst_destroy(list);
    puts("destroy");
    exit(EXIT_SUCCESS);
}
