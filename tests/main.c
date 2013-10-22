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
    h = init_hashtable(2);
    add(h, key, value, 0);
    add(h, value, key, 0);
    /* add(h, "diogo", "martins", 1); */
    par = get(h, key, 0);
    if(par != NULL)
        printf("key: %s, value: %s\n", par->key, par->value);
    else
        puts("erro");
    delete_hashtable(h);

    exit(EXIT_SUCCESS);
}

