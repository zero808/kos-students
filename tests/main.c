#include <stdlib.h>
#include <stdio.h>
#include <hashtable.h>
#include <list.h>
#include <kos_client.h>

int main(int argc, char *argv[])
{
    hashtable *h = NULL;
    char key[3] = "ab";
    char value[3] = "cd";
    KV_t *par = NULL;
    h = init_hashtable(KV_SIZE);
    add(h, key, value);
    add(h, "ba", value);
    add(h, value, key);
    par = get(h, key);
    if(par != NULL)
        printf("key: %s, value: %s\n", par->key, par->value);
    else
        puts("erro");
    add(h, key, "martins");
    par = get(h, key);
    if(par != NULL)
        printf("key: %s, value: %s\n", par->key, par->value);
    else
        puts("erro");
    lst_print(h->lists[hash(key)]);
    lst_print(h->lists[hash(value)]);
    delete_hashtable(h);

    exit(EXIT_SUCCESS);
}

