/*
 * list.h - definitions and declarations of the integer list 
 */
#ifndef __LIST_H__
#define __LIST_H__

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <kos_client.h>


/* lst_iitem - each element of the list points to the next element */
typedef struct lst_iitem {
   KV_t *item;
   struct lst_iitem *next;
} lst_iitem_t;

/* list_t */
typedef struct {
   lst_iitem_t * first;
} list_t;



/* lst_new - allocates memory for list_t and initializes it */
list_t* lst_new();

/* lst_destroy - free memory of list_t and all its items */
void lst_destroy(list_t *);

/* lst_insert - insert a new item with value 'value' in list 'list' */
void lst_insert(list_t *list, char *key, char *value);

/* lst_remove - remove first item of value 'value' from list 'list' */
int lst_remove(list_t *list, char *key);

/* lst_print - print the content of list 'list' to standard output */
void lst_print(list_t *list);

/* Returns the size of the list */
int lst_size(list_t *list);

KV_t *lst_get(list_t *list, char* key);

#endif