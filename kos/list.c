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
   list->size = 0;
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

char* lst_insert(list_t *list, char *key, char* value)
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
                char *oldValue = calloc(KV_SIZE, sizeof(char));
                strncpy(oldValue, tempA->item->value, KV_SIZE);
                strncpy(tempA->item->value, value, KV_SIZE);
                return oldValue; /* the receiver should free the memory */
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
        list->size += 1;
    }
    return NULL;
}

/* A função lst_remove recebe uma lista e um valor e remove o primeiro item com aquele valor. */
char *lst_remove(list_t *list, char *key)
{
    char *ret = NULL;
    if(list->first == NULL) {
        return ret;
    }
    else {
        lst_iitem_t  *tempA, *tempB;
        tempA = list->first;
        /* checks first element */
        if(!strncmp(tempA->item->key, key, KV_SIZE)) {
            tempB = tempA->next;
            /* allocate memory for the copy of the element to be removed */
            ret = calloc((size_t) KV_SIZE, sizeof(char));
            if(ret==NULL) {
                fprintf(stderr, "Dynamic memory allocation failed\n");
                exit(EXIT_FAILURE);
            }
            strncpy(ret, tempA->item->value, KV_SIZE);
            /* free the memory of the element in the list */
            free(tempA->item);
            free(tempA);
            list->first = tempB;
            list->size -= 1;
            return ret;
        }
        else {
            while(tempA != NULL) {
                /* checks the next element */
                if(tempA->next != NULL) {
                    /* If we found the element we remove it */
                    if(!strncmp(tempA->next->item->key, key, KV_SIZE)) {
                        tempB = tempA->next->next;
                        ret = calloc((size_t) KV_SIZE, sizeof(char));
                        if(ret==NULL) {
                            fprintf(stderr, "Dynamic memory allocation failed\n");
                            exit(EXIT_FAILURE);
                        }
                        strncpy(ret, tempA->next->item->value, KV_SIZE);
                        free(tempA->next->item);
                        free(tempA->next);
                        tempA->next = tempB;
                        list->size -= 1;
                        return ret;
                    }
                    else {
                        /* go on to the next */
                        tempA = tempA->next;
                    }
                }
                else {
                    /* If we get here the element isn't on the list */
                    return ret;
                }
            }
        }
    }
    return ret; /* para o compilador nao se queixar */
}


void lst_print(list_t *list)
{
    lst_iitem_t *temp;
    for(temp = list->first; temp != NULL; temp = temp->next) {
        printf("key: %s, value: %s\n", temp->item->key, temp->item->value);
    }
}

int lst_size(list_t *list) {
    return list->size;
}

char *lst_get(list_t *list, char* key) {
    lst_iitem_t  *temp;

    temp = list->first;
    while(temp != NULL) {
        if(!strncmp(key, temp->item->key, KV_SIZE)) {
            return temp->item->value;
        }
        else
            temp = temp->next;
    }
    return NULL;
}
