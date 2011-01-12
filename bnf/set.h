#ifndef SET_H
#define SET_H

#pragma once
#include "stdafx.h"
#include "slist.h"

typedef struct _cset
{
  bool(*match_fn)(const void*,void*);
  slist* members;
} cset;

bool comp_str(const void* a, void* b);

cset* new_set(bool(*comp_fn)(const void*,void*));
bool cset_add(cset* set,
              void* member);
bool cset_remove(cset* set,
                 void* member);
cset* cset_intersection(cset* a,
                        cset* b);
cset* cset_difference(cset* a,
                      cset* b);
cset* cset_union_add(cset* a,
                     cset* b);
cset* cset_union(cset* a,
                 cset* b);
//bool cset_union(cset* a,
//                cset* b);

bool cset_is_intersection(cset* a,
                          cset* b);
bool cset_is_difference(cset* a,
                        cset* b);
bool cset_is_superset(cset* a,
                      cset* b);
bool cset_is_subset(cset* a,
                    cset* b);
bool cset_is_member(cset* a,
                    void* b);
void* cset_get_first_match(cset* a,
                           void* match);
bool cset_remove_first_match(cset* a,
                             void* match);
cset* cset_copy(cset* a);
void cset_clear(cset* a,
                void(*dealloc)(void*));
void cset_destroy(cset* set);
void print_set(cset* set,
               char* item_format);

#endif