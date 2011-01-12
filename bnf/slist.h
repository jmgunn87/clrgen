#ifndef SLIST_H
#define SLIST_H

#pragma once 
#include "stdafx.h"

typedef struct _slist_elem
{
  void* _data;
  struct _slist_elem* _next;
} slist_elem;

typedef struct _slist
{
  size_t _size;
  slist_elem* _head;
  slist_elem* _tail;
} slist;

slist* new_slist();

void slist_append(slist* list,
                  void* data);

void slist_append_at(slist* list,
                     int index,
                     void* data);

void* slist_at(slist* list,
               int index);

unsigned char slist_remove_at(slist* list,
                              int index);

void* slist_pop(slist* list);
void* slist_peek(slist* list);
void* slist_peekn(slist* list,
                  int n);
void slist_push(slist* list,
                void* data);

void slist_remove_tail(slist* list);

void* slist_find(slist* list,
                 int* index,
                 void* comp_dat,
                 char(*comp_fn)(void*,void*));

slist* slist_split(slist* list,
                   int index);

slist_elem* slist_tail(slist* list);

void slist_clear(slist* list,
                 void(*dealloc)(void*));

slist* slist_copy(slist* list);

void slist_destroy(slist* list);

void print_slist(slist* list,
                 char* item_format);
#endif