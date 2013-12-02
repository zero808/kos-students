#include <list.h>

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

lst_ret_t *lst_insert(list_t *list, char *key, char* value, int file_position)
{
    lst_ret_t *ret = NULL;
    if((list != NULL) && (key != NULL) && (value != NULL)) {
        lst_iitem_t  *tempA, *tempB;
        ret = calloc(1, sizeof(lst_ret_t));

        /* Traverse the list */
        for(tempA = list->first, tempB = list->first; tempA != NULL; tempA = tempA->next) {
            /* to ensure that tempB will end one position behind tempA */
            tempB = tempA;
            /* check if there's an item using the key */
            if(strncmp(key, tempA->item->key, KV_SIZE) == 0) {
                /* if it already exists replace its value */
                ret->value = calloc(KV_SIZE, sizeof(char));
                strncpy(ret->value, tempA->item->value, KV_SIZE);
                ret->position = tempA->file_position;
                strncpy(tempA->item->value, value, KV_SIZE);
                return ret; /* the receiver should free the memory */
            }
        }
        /* if there's time, change this to adding elements at the
         * beggining of the list, there's no need for inserting this in
         * any order */

        /* If there isn't an item using this key we create a new item and
            * insert it at the end */
        tempA =  malloc(sizeof(lst_iitem_t));
        tempA->item = calloc(1, sizeof(KV_t));
        strncpy(tempA->item->key, key, KV_SIZE);
        strncpy(tempA->item->value, value, KV_SIZE);
        tempA->file_position = file_position;
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
        ret->value = NULL;
        ret->position = file_position;
    }
    return ret;
}

/* we have this function in order to avoid writing more code
 h
 * this is going to be used to track which positions in the file
 * are no longer relevant due to removing a certain element */
int lst_insert_pos(list_t *list, int file_position)
{
    if(list != NULL) {
        /* we always want to add a new item
         * and for simplicity we add it at the beggining */
        lst_iitem_t* item = NULL;

        item = calloc(1, sizeof(lst_iitem_t));
        if(item==NULL) {
            fprintf(stderr, "\ndynamic memory allocation failed\n");
            exit(EXIT_FAILURE);
        }
        item->item = NULL;
        item->file_position = file_position;
        item->next = list->first;
        list->first = item->next;
        list->size += 1;
    }
    return 0;
}

int lst_remove_pos(list_t *list)
{
    int ret;
    lst_iitem_t *iterA = NULL, *iterB = NULL;
    if(list != NULL) {
        /* check the first */
        iterA = list->first;
        if(iterA != NULL) {
            ret = iterA->file_position;
            iterB = iterA->next;
            list->first = iterB;
            free(iterA);
        }
    }
    return ret;
}

/* A função lst_remove recebe uma lista e um valor e remove o primeiro item com aquele valor. */
lst_ret_t *lst_remove(list_t *list, char *key)
{
    lst_ret_t *ret = NULL;
    /* make this a double linked list ffs */
    if(list != NULL) {
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
                ret = calloc(1, sizeof(lst_ret_t));
                if(ret==NULL) {
                    fprintf(stderr, "Dynamic memory allocation failed\n");
                    exit(EXIT_FAILURE);
                }
                ret->value = calloc(KV_SIZE, sizeof(char));
                strncpy(ret->value, tempA->item->value, KV_SIZE);
                ret->position = tempA->file_position;
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
                            strncpy(ret->value, tempA->next->item->value, KV_SIZE);
                            ret->position = tempA->next->file_position;
                /* free the memory of the element in the list */
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

lst_ret_t *lst_get(list_t *list, char* key) {
    lst_ret_t *ret = NULL;
    lst_iitem_t  *temp = NULL;

    temp = list->first;
    while(temp != NULL) {
        if(!strncmp(key, temp->item->key, KV_SIZE)) {
            ret = calloc(1, sizeof(lst_ret_t));
            if(ret==NULL) {
                fprintf(stderr, "Dynamic memory allocation failed\n");
                exit(EXIT_FAILURE);
            }
            ret->value = calloc(KV_SIZE, sizeof(char));
            strncpy(ret->value, temp->item->value, KV_SIZE);
            ret->position = temp->file_position;
                /* free the memory of the element in the list */
            /* TODO: this return is most likely unneeded */
            return ret;
        }
        else
            temp = temp->next;
    }
    return ret;
}
