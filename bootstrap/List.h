#ifndef __recode__list_h__
#define __recode__list_h__

typedef struct List List;
typedef struct List_Iterator List_Iterator;

List *list__create();

void list__append(List *self, void *item);

List_Iterator *list__create_iterator(List *self);

int list_iterator__has_next(List_Iterator *self);

void *list_iterator__current(List_Iterator *self);

void *list_iterator__next(List_Iterator *self);

void list_iterator__save_state(List_Iterator *self);

void list_iterator__restore_state(List_Iterator *self);

#endif
