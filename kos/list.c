/* Sistemas Operativos 1º Semestre 2012/2013
 *  Grupo 82:
 *  Duarte Barreira - 64752
*/

/*
 * list.c - implementation of the integer list functions
 */

#include <list.h>

/* list_t* lst_new(int amount) */
/* { */
/*    int ix; */
/*    list_t *list; */
/*    list = (list_t*) malloc(sizeof(list_t) * amount); */
/*    list->amount = amount; */

/*    for(ix = 0; ix < amount ; ++ix) { */
/*        list[ix].first = NULL; */
/*    } */
/*    return list; */
/* } */
list_t* lst_new()
{
   list_t *list;
   list = (list_t*) malloc(sizeof(list_t));

   list->first = NULL;
   return list;
}

void lst_destroy(list_t *list)
{
    if(list->first != NULL) {
        lst_iitem_t  *tempA;
        /* Sem o if acima nao poderiamos a proxima instruçao causaria erros*/
       tempA = list->first->next;

            while(list->first != NULL) {
                free(list->first->item);
                free(list->first);
                list->first = tempA;
                /* o mesmo que em cima */
                if(tempA != NULL) {
                    tempA = list->first->next;
                }
            }
        }
    free(list);
}


void lst_insert(list_t *list, char *key, char* value)
{
    if(list->first == NULL) {
        list->first = (lst_iitem_t*) malloc(sizeof(lst_iitem_t));
        list->first->item = (KV_t*) malloc(sizeof(KV_t));
        strcpy(list->first->item->key, key);
        strcpy(list->first->item->value, value);
        list->first->next = NULL;
    }
    else {
        lst_iitem_t  *tempA, *tempB;
        tempA = (lst_iitem_t*) malloc(sizeof(lst_iitem_t));
        list->first->item = (KV_t*) malloc(sizeof(KV_t));
        strcpy(tempA->item->key, key);
        strcpy(list->first->item->value, value);
        tempA->next = NULL;
        tempB = list->first;

        /* Percorrer a lista ate encontrar o ultimo elemento */
        while(tempB->next != NULL) {
            tempB = tempB->next;
        }
        /* adicioná-lo ao fim da lista */
        tempB->next = tempA;
    }
  /* printf("lst_insert: value=%d\n", value);*/
}

/* A função lst_remove recebe uma lista e um valor e remove o primeiro item com aquele valor. */
int lst_remove(list_t *list, char *key)
{
    if(list->first == NULL) {
        return -1;
    }
    else {
        lst_iitem_t  *tempA, *tempB;
        tempA = list->first;
        /* checks first element */
        if(!strcmp(tempA->item->key, key)) {
            tempB = tempA->next;
            free(tempA);
            list->first = tempB;
            return 0;
        }
        else {
            while(tempA != NULL) {
                /* checks the next element */
                if(tempA->next != NULL) {
                    /* If we found the element we remove it */
                    if(!strcmp(tempA->next->item->key, key)) {
                        tempB = tempA->next->next;
                        free(tempA->next);
                        tempA->next = tempB;
                        return 0;
                    }
                    else {
                        /* go on to the next */
                        tempA = tempA->next;
                    }
                }
                else {
                    /* If we get here the element isn't on the list */
                    return -1;
                }
            }
        }
    }
    return 0; /* para o compilador nao se queixar */
}


void lst_print(list_t *list)
{
    lst_iitem_t *temp;
    for(temp = list->first; temp != NULL; temp = temp->next) {
        printf("key: %s, value: %s\n", temp->item->key, temp->item->value);
    }
    puts("\n");
}

int lst_size(list_t *list) {
    lst_iitem_t *p = list->first;
    int counter= 0;
    while(p != NULL) {
        ++counter;
        p = p->next;
    }

    return counter;
}

KV_t *lst_get(list_t *list, char* key) {
    lst_iitem_t  *temp;

    temp = list->first;
    while(temp != NULL) {
        if(!strcmp(key, temp->item->value)) {
            return temp->item;
        }
        else
            temp = temp->next;
    }
    return NULL;
}
