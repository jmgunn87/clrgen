#pragma once 
#include "stdafx.h"
#include "slist.h"

slist* new_slist()
{
  slist* nsl=(slist*)malloc(sizeof(slist));
  memset((void*)nsl,0,sizeof(slist));
  nsl->_size=-1;
  return nsl;
}
void slist_append(slist* list,
                  void* data)
{
  slist_elem* pos=0;

  if(!list)return;

  //empty list, add an element
  if(!list->_head)
  {
    (*list)._head=(slist_elem*)malloc(sizeof(slist_elem));
    memset((void*)(*list)._head,0,sizeof(slist_elem));
    (*list)._head->_data=data;
    (*list)._tail=list->_head;
    (*list)._size=1;
    return;
  }
  else if(list->_size<1)
  {
    (*list)._head->_data=data;
    (*list)._tail=list->_head;
    (*list)._size=1;
    return;
  }

  /*add in support for caching*/
  pos=list->_tail;
  if(pos->_next)
  {
    (*(*pos)._next)._data=data;
    (*list)._tail=pos->_next;
    (*list)._size+=1;
    return;
  }
  /*allocate a new element*/
  (*pos)._next=(slist_elem*)malloc(sizeof(slist_elem));
  memset((void*)(*pos)._next,0,sizeof(slist_elem));
  (*(*pos)._next)._data=data;
  (*list)._tail=pos->_next;
  (*list)._size+=1;
  return;
}

void slist_append_at(slist* list,
                     int index,
                     void* data)
{
  int x=1;
  slist_elem *pos=0,
             *temp=0;
  
  if(!list)return;

  //empty list, add an element
  if(!list->_head)
  {
    (*list)._head=(slist_elem*)malloc(sizeof(slist_elem));
    memset((void*)(*list)._head,0,sizeof(slist_elem));
    (*list)._head->_data=data;
    (*list)._size=1;
    return;
  }
  //fast forward to the last element
  pos=list->_head;
  for(;pos->_next,x<=list->_size;pos=pos->_next,++x)
    if(x==index)
    {
      temp=(*pos)._next;
      (*pos)._next=(slist_elem*)malloc(sizeof(slist_elem));
      memset((void*)(*pos)._next,0,sizeof(slist_elem));
      (*(*pos)._next)._data=data;
      (*(*pos)._next)._next=&*temp;
      (*list)._size+=1;
    }
  return;
}

void* slist_at(slist* list,
               int index)
{
  slist_elem* pos=list->_head;
  int x=1;
  if(!list||!list->_size||index>list->_size)return 0;
  //iterate
  for(;pos->_next,x<=list->_size;pos=pos->_next,++x)
    if(x==index)return pos->_data;
  return 0;
}

unsigned char slist_remove_at(slist* list,
                              int index)
{
  int x=1;
  slist_elem* pos=list->_head;
  slist_elem* temp=0;
  if(!list||!list->_size||index>list->_size)return 0;
  //iterate to one before element and remove the next element
  for(;pos->_next,x<list->_size;pos=pos->_next,++x)
    if(x==index-1)
    {
      if(!pos->_next)return 0;
      if(pos->_next&&pos->_next->_next)
        temp=pos->_next->_next;
      (*pos)._next->_data=0;
      free((*pos)._next);
      (*pos)._next=temp?temp:0;
      (*list)._size=(*list)._size>0?(*list)._size-=1:0;
      return 1;
    }
  return 0;
}

void* slist_pop(slist* list)
{
  slist_elem* h=0;
  void* data=0;
  if(!list||list->_size<1)
    return 0;
  h=list->_head;
  data=list->_head->_data;
  (*list)._head=h->_next;
  --(*list)._size;

  free(h);
  return data;
}

void* slist_peek(slist* list)
{
  if(!list||list->_size<1)
    return 0;
  return list->_head->_data;
}

void* slist_peekn(slist* list,
                  int n)
{
  return slist_at(list,n);
}

void slist_push(slist* list,
                void* data)
{
  slist_elem* new_elem=0;

  if(!list)
    return;
  if(!list->_head)
    (*list)._size=0;

  new_elem=(slist_elem*)malloc(sizeof(slist_elem));
  (*new_elem)._data=data;
  (*new_elem)._next=list->_head;
  (*list)._head=new_elem;
  ++(*list)._size;
}

void slist_remove_tail(slist* list)
{
  if(!list||list->_size<1)return;
  //slist_elem* iter=list->_head;
  //for(;iter&&iter->_next->_next;iter=iter->_next);
  //if(iter->_next)
  //{
  //  iter->_next->_data=0;
  //  free(iter->_next);
  //  iter->_next=0;
  //  --(*list)._size;
  //}
  //else
  //{
  //  iter->_data=0;
  //  free(iter);
  //  (*list)._head=0;
    --(*list)._size;
  //}
  
}

void* slist_find(slist* list,
                 int* index,
                 void* comp_dat,
                 char(*comp_fn)(void*,void*))
{
  int i=1;
  slist_elem* sle=0;

  if(!list||list->_size<1||!comp_dat||!comp_fn)
    return 0;
  sle=list->_head;
  for(;sle;sle=sle->_next,++i)
    if(comp_fn(sle->_data,comp_dat)==1)
    {
      if(index)*index=i;
      return sle->_data;
    }
  return 0;
}

slist* slist_split(slist* list,
                   int index)
{
  slist_elem* temp=0;
  slist* split=0;
  slist_elem* pos=0;
  int i=1;

  if(!list||list->_size<1||index>list->_size)
    return 0;
  
  split=new_slist();
  pos=list->_head;
  //iterate to one before element and remove the next element
  for(;pos->_next,i<list->_size;pos=pos->_next,++i)
    if(i==index-1)
    {
      temp=pos->_next;
      pos->_next=0;
      split->_head=temp;

      split->_size=list->_size-index;
      list->_size=index;

      return split;
    }
  return 0;
}


slist_elem* slist_tail(slist* list)
{
  return list?list->_tail:0;
}

void slist_clear(slist* list,
                 void(*dealloc)(void*))
{
  if(!list||!list->_size)
    return;
  if(!dealloc)
  {
    (*list)._size=0;
    (*list)._tail=0;
    return;
  }
}

slist* slist_copy(slist* list)
{
  int i=1;
  slist* ret=0;
  slist_elem* sle=0;

  if(!list)return 0;
  
  ret=new_slist();
  sle=list->_head;
  for(;sle&&i<list->_size;sle=sle->_next,++i)
    slist_append(list,(void*)sle->_data);
  return ret;
}

void slist_destroy(slist* list)
{
  slist_elem  *sl_curr=list->_head,
              *sl_next=0;
  while(sl_curr&&sl_curr->_next)
  {
    sl_next=sl_curr->_next;
    sl_curr->_next=0;
    sl_curr->_data=0;
    free(sl_curr);
    sl_curr=sl_next;
  }
  list->_head=0;
  list->_size=-1;
}

void print_slist(slist* list,
                 char* item_format)
{
  slist_elem* sle=list->_head;
  for(;sle;sle=sle->_next)
    printf(item_format,sle->_data);
}