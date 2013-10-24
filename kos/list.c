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


/* void lst_insert(list_t *list, char *key, char* value) */
/* { */
/*     if((list != NULL) && (key != NULL) && (value != NULL)) { */
/*         /1* if the list is empty just add it *1/ */
/*         if(list->first == NULL) { */
/*             list->first =  malloc(sizeof(lst_iitem_t)); */
/*             list->first->item =  malloc(sizeof(KV_t)); */
/*             strncpy(list->first->item->key, key, KV_SIZE); */
/*             strncpy(list->first->item->value, value, KV_SIZE); */
/*             list->first->next = NULL; */
/*         } */
/*         else { */
/*             lst_iitem_t  *temp; */

/*             /1* Traverse the list *1/ */
/*             for(temp = list->first; temp != NULL; temp = temp->next) { */
/*                 /1* check if there's an item using the key *1/ */
/*                 if(strncmp(key, temp->item->key, KV_SIZE) == 0) { */
/*                     /1* if it already exists replace its value *1/ */
/*                     strncpy(temp->item->value, value, KV_SIZE); */
/*                     return; */
/*                 } */
/*             } */
/*             /1* If there isn't an item using this key we create a new item and */
/*              * insert it at the end *1/ */
/*             temp =  malloc(sizeof(lst_iitem_t)); */
/*             temp->item = calloc(1, sizeof(KV_t)); */
/*             strncpy(temp->item->key, key, KV_SIZE); */
/*             strncpy(temp->item->value, value, KV_SIZE); */
/*             temp->next = NULL; */
/*         } */
/*     } */
/* } */
void lst_insert(list_t *list, char *key, char* value)
{
    if((list != NULL) && (key != NULL) && (value != NULL)) {
        lst_iitem_t  *tempA, *tempB;

        /* Traverse the list */
        for(tempA = list->first, tempB = list->first; tempA != NULL; tempA = tempA->next) {
            /* to ensure that tempB will end one position behind tempA */
            tempB = tempA;
            /* check if there's an item using the key */
            if(strncmp(key, tempA->item->key, KV_SIZE) == 0) {
                /* if it already exists replace its value */
                strncpy(tempA->item->value, value, KV_SIZE);
                return;
            }
        }
        /* If there isn't an item using this key we create a new item and
            * insert it at the end */
        tempA =  malloc(sizeof(lst_iitem_t));
        tempA->item = calloc(1, sizeof(KV_t));
        strncpy(tempA->item->key, key, KV_SIZE);
        strncpy(tempA->item->value, value, KV_SIZE);
        tempA->next = NULL;
        /* if tempB is NULL the list is empty */
        if(tempB == NULL) {
            list->first = tempA;
        }
        /* otherwise tempB is the last element */
        else {
            tempB->next = tempA;
        }
    }
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
        if(!strncmp(tempA->item->key, key, KV_SIZE)) {
            tempB = tempA->next;
            free(tempA->item);
            free(tempA);
            list->first = tempB;
            return 0;
        }
        else {
            while(tempA != NULL) {
                /* checks the next element */
                if(tempA->next != NULL) {
                    /* If we found the element we remove it */
                    if(!strncmp(tempA->next->item->key, key, KV_SIZE)) {
                        tempB = tempA->next->next;
                        free(tempA->next->item);
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
        if(!strncmp(key, temp->item->key, KV_SIZE)) {
            return temp->item;
        }
        else
            temp = temp->next;
    }
    return NULL;
}
