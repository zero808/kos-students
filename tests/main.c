
#include <stdlib.h>
#include <stdio.h>
#include <hashtable.h>
#include <kos_client.h>

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  main
 *  Description:  
 * =====================================================================================
 */
int main(int argc, char *argv[])
{
    hashtable *h;
    KV_t *par;
    puts("l");
    init_hashtable(h, 2);
    puts("l");
    add(h, "duarte", "miguel", 0);
    add(h, "diogo", "martins", 0);
    /* add(h, "diogo", "martins", 1); */
    par = get(h, "duarte", 0);
    printf("key: %s, value: %s\n", par->key, par->value);
    delete_hashtable(h);

    exit(EXIT_SUCCESS);
}
