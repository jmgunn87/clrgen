#include "stdafx.h"
#include "set.h"

bool comp_str(const void* a, void* b)
{
  return strcmp((const char*)a,(const char*)b)==0;
}

cset* new_set(bool(*comp_fn)(const void*,void*))
{
  cset* ret=(cset*)malloc(sizeof(cset));
  ret->match_fn=comp_fn;
  ret->members=new_slist();
  ret->members->_size=0;
  return ret;
}
bool cset_add(cset* set,
              void* member)
{
  int i=0;
  if(!set||!set->members||!set->match_fn)return false;
  slist_elem* temp=set->members->_head;
  if(!temp)
  {
    slist_append((*set).members,member);
    return true;
  }
  while(temp&&i<set->members->_size)
  {
    if(set->match_fn(member,temp->_data))
      return false;
    temp=temp->_next;
    ++i;
  }
  slist_append((*set).members,member);
  return true;
}

bool cset_remove(cset* set,
                 void* member)
{
  if(!set||!set->match_fn)return false;
  slist_elem* temp=set->members->_head;
  if(!temp)return false;
  int index=1;
  do
  {
    if(set->match_fn(member,temp->_data))
    {
      slist_remove_at(set->members,index);
      return true;
    }
    ++index;
    temp=temp->_next;
  }
  while(temp);
  return false;
}

cset* cset_intersection(cset* a,
                        cset* b)
{
  if(!a||!b||!a->match_fn)return 0;
  slist_elem* atemp=a->members->_head;
  slist_elem* btemp=b->members->_head;
  if(!atemp||!btemp)
    return 0;
  
  cset* i=new_set(a->match_fn);
  while(btemp)
  {
    while(atemp)
    {
      if(i->match_fn(atemp->_data,btemp->_data))
        cset_add(i,atemp->_data);
      atemp=atemp->_next;
    }
    atemp=a->members->_head;
    btemp=btemp->_next;
  }
  return i;
}

cset* cset_difference(cset* a,
                      cset* b)
{
  if(!a||!b||!a->match_fn)return 0;
  slist_elem* atemp=a->members->_head;
  slist_elem* btemp=b->members->_head;
  if(!atemp||!btemp)
    return 0;

  cset* d=new_set(a->match_fn);
  bool is_mem=false;
  while(atemp)
  {
    is_mem=false;
    while(btemp)
    {
      if(d->match_fn(atemp->_data,btemp->_data))
      {
        is_mem=true;
        break;
      }
      btemp=btemp->_next;
    }
    if(!is_mem)cset_add(d,atemp->_data);
    btemp=b->members->_head;
    atemp=atemp->_next;
  }
  return d;
}
cset* cset_union_add(cset* a,
                     cset* b)
{
  if(!b)
    return a;
  if(!a||a->members->_size<1||!a->match_fn)
    return a;
  slist_elem* btemp=b->members->_head;
  if(!btemp)
    return a;
  while(btemp)
  {
    cset_add(a,btemp->_data);
    btemp=btemp->_next;
  }
  return a;
}

cset* cset_union(cset* a,
                 cset* b)
{
  if(!a||!b||!a->match_fn)
    return 0;

  slist_elem* atemp=a->members->_head;
  slist_elem* btemp=b->members->_head;

  if(!atemp)
    return b;
  if(!btemp)
    return a;

  cset* u=new_set(a->match_fn);
  while(btemp)
  {
    cset_add(u,btemp->_data);
    btemp=btemp->_next;
  }
  while(atemp)
  {
    cset_add(u,atemp->_data);
    atemp=atemp->_next;
  }
  return u;
}
bool cset_is_intersection(cset* a,
                          cset* b)
{
  if(!a||!b||!a->match_fn)return 0;
  slist_elem* atemp=a->members->_head;
  slist_elem* btemp=b->members->_head;
  if(!atemp||!btemp)
    return 0;

  while(btemp)
  {
    while(atemp)
    {
      if(a->match_fn(atemp->_data,btemp->_data))
        return true;
      atemp=atemp->_next;
    }
    atemp=a->members->_head;
    btemp=btemp->_next;
  }
  return false;
}
bool cset_is_difference(cset* a,
                        cset* b)
{
  if(!a||!b||!a->match_fn)
    return 0;
  if(a->members->_size<0||b->members->_size<0)
    return false;
  if(a->members->_size!=b->members->_size)
    return true;

  slist_elem* atemp=a->members->_head;
  slist_elem* btemp=b->members->_head;
  if(!atemp||!btemp)
    return 0;

  bool is_mem=false;
  while(atemp)
  {
    is_mem=false;
    while(btemp)
    {
      if(a->match_fn(atemp->_data,btemp->_data))
      {
        is_mem=true;
        break;
      }
      btemp=btemp->_next;
    }
    if(!is_mem)return true;
    btemp=b->members->_head;
    atemp=atemp->_next;
  }
  return false;
}
bool cset_is_superset(cset* a,
                      cset* b)
{
  return cset_is_subset(b,a);
}
bool cset_is_subset(cset* a,
                    cset* b)
{
  slist_elem* temp=b->members->_head;
  while(temp)
  {
    if(!cset_is_member(a,temp->_data))
      return false;
    temp=temp->_next;
  }
  return a->members->_size>b->members->_size;
}
bool cset_is_member(cset* a,
                    void* b)
{
  if(!a||!b||!a->match_fn)return false;
  slist_elem* temp=a->members->_head;
  while(temp)
  {
    if(a->match_fn(b,temp->_data))
      return true;
    temp=temp->_next;
  }
  return false;
}
void* cset_get_first_match(cset* a,
                           void* match)
{
  if(!a||!match||!a->match_fn)return 0;
  int i=0;
  slist_elem* temp=a->members->_head;
  while(temp&&i<a->members->_size)
  {
    if(a->match_fn(match,temp->_data))
      return temp->_data;
    temp=temp->_next;
    ++i;
  }
  return 0;
}
bool cset_remove_first_match(cset* a,
                             void* match)
{
  if(!a||!match||!a->match_fn)return 0;
  unsigned int index=1;
  slist_elem* temp=a->members->_head;
  while(temp)
  {
    if(a->match_fn(match,temp->_data))
    {
      slist_remove_at(a->members,index);
      return true;
    }
    ++index;
    temp=temp->_next;
  }
  return false;
}

  //cset* ret=new_set(a->match_fn);
  //for(slist_elem* sle=a->members->_head;sle;sle=sle->_next)
  //  slist_append(ret->members,(void*)sle->_data);
  ////return ret;
  //cset* ret=new_set(a->match_fn);
  //ret->members=slist_copy(a->members);
  //return ret;
cset* cset_copy(cset* a)
{
  cset* ret=new_set(a->match_fn);
  int i=0;
  slist_elem* sle=a->members->_head;
  for(;sle&&i<a->members->_size;sle=sle->_next,++i)
    slist_append(ret->members,(void*)sle->_data);
  return ret;
}
void cset_clear(cset* a,
                void(*dealloc)(void*))
{
  (*a).match_fn=0;
  slist_clear(a->members,dealloc);
}
void cset_destroy(cset* set)
{
  set->match_fn=0;
  slist_destroy(set->members);
}
void print_set(cset* set,
               char* item_format)
{
  print_slist(set->members,item_format);
}
