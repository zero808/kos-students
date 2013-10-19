#include <stdlib.h>
#include <stdio.h>
#include <hashtable.h>
#include <list.h>
#include <kos_client.h>

int main(int argc, char *argv[])
{
    hashtable *h;
    char key[3] = "ab";
    char value[3] = "cd";
    KV_t *par = NULL;
    puts("l");
    init_hashtable(h, 2);
    puts("l");
    add(h, "duarte", "miguel", 0);
    add(h, "diogo", "martins", 0);
    /* add(h, "diogo", "martins", 1); */
    par = get(h, "duarte", 0);
    if(par != NULL)
        printf("key: %s, value: %s\n", par->key, par->value);
    else
        puts("erro");
    delete_hashtable(h);

    exit(EXIT_SUCCESS);
}

