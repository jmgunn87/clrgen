/**************************************************
 *------------------------------------------------*
 *          NanoLR PARSER GENERATOR v1.0          *
 *------------------------------------------------*
 **************************************************/

#include "stdafx.h"
#include "bnf_constructs.h"

bnf_index* new_bnf_index(int i,
                         int p,
                         int pp,
                         cset* lookaheads)
{
  bnf_index* bi=(bnf_index*)malloc(sizeof(bnf_index));
  bi->item=i;
  bi->product=p;
  bi->product_part=pp;
  lookaheads?
    bi->lookaheads=cset_copy(lookaheads):
    bi->lookaheads=new_set(compare_bnf_lookaheads);
  return bi;
}
void clear_bnf_index(bnf_index* bi)
{
  if(!bi)return;
  cset_clear((*bi).lookaheads,0);
}
void destroy_bnf_index(bnf_index* bi)
{
  cset_destroy(bi->lookaheads);
  free(bi);
}
bnf_grammar* new_bnf_grammar()
{
  bnf_grammar* bnf=(bnf_grammar*)malloc(sizeof(bnf_grammar));
  memset((void*)bnf,0,sizeof(bnf_grammar));
  return bnf;
}

bool compare_bnf_lookaheads(const void* a,void* b)
{
  return (int)a==(int)b;
}

bool compare_bnf_index(const void* a,void* b)
{
  return ((bnf_index*)a)->item==((bnf_index*)b)->item&&
         ((bnf_index*)a)->product==((bnf_index*)b)->product&&
         ((bnf_index*)a)->product_part==((bnf_index*)b)->product_part;
}

bool compare_bnf_index_lookaheads(const void* a,void* b)
{
  if(cset_is_difference(((bnf_index*)a)->lookaheads,((bnf_index*)b)->lookaheads))
    return false;
  return compare_bnf_index(a,b);
}

bool compare_bnf_item_sets(const void* a,void* b)
{
  return !cset_is_difference((cset*)a,(cset*)b);
}

bool compare_bnf_item_kernels(const void* a, void* b)
{
  cset* ak=bnf_get_item_kernels((cset*)a);
  cset* bk=bnf_get_item_kernels((cset*)b);
  if(!ak||!bk)
    return false;
  if(cset_is_difference(ak,bk))
  {
    cset_destroy(ak);
    cset_destroy(bk);
    return false;
  }
  cset_destroy(ak);
  cset_destroy(bk);
  return true;
}

bool compare_bnf_item_kernels_lookaheads(const void* a, void* b)
{
  cset* ak=bnf_get_item_kernels((cset*)a);
  cset* bk=bnf_get_item_kernels((cset*)b);
  if(!ak||!bk)
    return false;
  ak->match_fn=compare_bnf_index_lookaheads;
  if(cset_is_difference(ak,bk))
  {
    cset_destroy(ak);
    cset_destroy(bk);
    return false;
  }
  cset_destroy(ak);
  cset_destroy(bk);
  return true;
}

int bnf_index_of_symbol(bnf_grammar* bnf,
                        char* item)
{
  if(!item)return -1;
  int i=0;
  for(;i<bnf->symbol_count;++i)
    if(strcmp(bnf->lexicon[i],item)==0)
      return i;
  return -1;
}

/********************************
 * convert a parsed list into a *
 * grammar structure.           *
 ********************************/
int convert_to_bnfg(bnf_grammar* bnf,
                    slist* items,
                    cset* products,
                    cset* tokens,
                    cset* actions)
{
  int j=0;
  slist_elem* sle=0;

  if(!bnf||!items||!products||!tokens||!actions)
    return 0;

  (*bnf).symbol_count=(tokens->members->_size<0?0:tokens->members->_size)+
                      (products->members->_size<0?0:products->members->_size)+
                      (actions->members->_size<0?0:actions->members->_size);
  (*bnf).lexicon=(char**)calloc(bnf->symbol_count,sizeof(char*));

  
  sle=products->members->_head;
  while(sle)
  {
    (*bnf).lexicon[j]=(char*)sle->_data;
    sle=sle->_next;
    ++j;
  }
  (*bnf).start_of_terms=j;
  sle=tokens->members->_head;
  while(sle)
  {
    (*bnf).lexicon[j]=(char*)sle->_data;
    //printf("%s\n",(char*)sle->_data);
    sle=sle->_next;
    ++j;
  }
  (*bnf).start_of_actions=j;
  sle=actions->members->_head;
  while(sle)
  {
    (*bnf).lexicon[j]=(char*)malloc(strlen((char*)sle->_data));
    strcpy((*bnf).lexicon[j],(char*)sle->_data);
    (*bnf).lexicon[j][strlen((char*)sle->_data)]='\0';
    //printf("%s\n",sle->_data);
    sle=sle->_next;
    ++j;
  }
  j=0;
  (*bnf).item_count=items->_size==-1?0:items->_size;
  (*bnf).items=(int***)calloc(bnf->item_count,sizeof(int**));
  memset((void*)(*bnf).items,-3,bnf->item_count*sizeof(int**));
  sle=items->_head;
  while(sle)
  {
    (*bnf).items[j]=bnf_index_parse_item(bnf,(bnf_parse_item*)sle->_data);
    sle=sle->_next;
    ++j;
  }

  cset_destroy(tokens);
  cset_destroy(products);
  cset_destroy(actions);

  return 1;
}

bnf_parse_item* new_bnf_parse_item(char* head)
{
  bnf_parse_item* bpi=(bnf_parse_item*)malloc(sizeof(bnf_parse_item));
  if(head)
  {
    bpi->head=(char*)malloc(strlen(head)+1);
    sprintf(bpi->head,"%s\0",head);
  }
  else bpi->head=0;
  bpi->products=new_slist();
  return bpi;
}

int** bnf_index_parse_item(bnf_grammar* bnf,
                           bnf_parse_item* item)
{
  int j=0;
  int i=1;
  int** i_item=(int**)calloc(item->products->_size+1,sizeof(int*));
  i_item[0]=(int*)calloc(2,sizeof(int));
  i_item[0][1]=item->products->_size;//store the number of products
  i_item[0][0]=bnf_index_of_symbol(bnf,item->head);

  //now cycle through the list and store each of its products
  slist_elem* sle=item->products->_head;
  slist_elem* sle2=0;
  while(sle)
  {
    i_item[i]=(int*)calloc(((slist*)sle->_data)->_size+2,sizeof(int));
    int k=0;
    for(k=0;k<((slist*)sle->_data)->_size+2;++k)
      i_item[i][k]=-3;

    sle2=((slist*)sle->_data)->_head;
    while(sle2)
    {
      if((char*)sle2->_data)
        if(*(char*)sle2->_data=='{')//now reindex actions
        {
          i_item[i][j]=-2;//add an end of product that defines an action next
          i_item[i][j+1]=bnf_index_of_symbol(bnf,(char*)sle2->_data);
          sle2=sle2->_next;
          ++j;
          continue;
        }
      i_item[i][j]=bnf_index_of_symbol(bnf,(char*)sle2->_data);      
      sle2=sle2->_next;
      ++j;
    }
    i_item[i][++j]=-1;
    j=0;
    sle=sle->_next;
    ++i;
  }
  return i_item;
}
void initialize_first_array(bnf_grammar* bnf)
{
  first_items=(cset**)calloc(bnf->start_of_actions+1,sizeof(cset*));
  int i=0;
  for(;i<=bnf->start_of_actions;++i)
    first_items[i]=new_set(compare_bnf_lookaheads);
}

int bnf_first(bnf_grammar* bnf,
              int symbol)
{
  //make sure the first array has been initalized
  if(!first_items)
    initialize_first_array(bnf);

  if(symbol<0||symbol>=bnf->start_of_actions)return -1;

  //deal with a terminal here
  if(IS_TERM(bnf,symbol))
  {
    cset_add(first_items[symbol],(void*)symbol);
    return symbol;
  }

  //now deal with non-terms by recursing
  int j=1;
  int k=0;
  int index=0;
    for(;j<=bnf->items[symbol][0][1];++j)//product
      for(k=0;bnf->items[symbol][j][k]!=-1;++k)//productpart
      {
        if(bnf->items[symbol][j][k]==symbol)break;
        index=bnf_first(bnf,bnf->items[symbol][j][k]);
        if(index>0&&index!=symbol)
        {
          first_items[symbol]=cset_union(first_items[symbol],first_items[index]);
          break;
        }
      }
  return symbol;
}
void bnf_generate_first_symbols(bnf_grammar* bnf)
{
  //make sure the first array has been initalized
  if(!first_items)
    initialize_first_array(bnf);
  int i=0;
  for(;i<bnf->start_of_actions;++i)
  {
     bnf_first(bnf,i);
     //if(i<bnf->start_of_terms)
     //{
     //  printf("FIRST(%s)::",bnf->lexicon[i]);
     //  print_set(first_items[i],"%d,");
     //  printf("\n");
     //}
  }
}
cset* bnf_get_first_set(bnf_grammar* bnf, int symbol)
{
  if(!first_items)
    bnf_generate_first_symbols(bnf);
  return symbol>=0&&symbol<bnf->start_of_actions?first_items[symbol]:0;
}

bool add_lookaheads_to_item(cset* item,
                            bnf_index* ind)
{
  int i=0;
  int sz_before=0;
  slist_elem* sle=item->members->_head;
  for(;sle&&i<item->members->_size;++i,sle=sle->_next)
    if(COMPARE_INDEX_PTRS(((bnf_index*)sle->_data),ind))
    {
      sz_before=((bnf_index*)sle->_data)->lookaheads->members->_size;
      ((bnf_index*)sle->_data)->lookaheads=
        cset_union_add(((bnf_index*)sle->_data)->lookaheads,ind->lookaheads);
      if(((bnf_index*)sle->_data)->lookaheads->members->_size>sz_before)
        return true;
    }
  return false;
}

void bnf_kernel_items(bnf_grammar* bnf,
                      bnf_index* i,
                      cset* closure)
{
  if(!bnf)return;

  int current=bnf->items[i->item][i->product][i->product_part];
  if(current>bnf->start_of_actions||current<0||IS_TERM(bnf,current))
    return;
  
  int sym=0;
  ++i->product_part;
  sym=PART_SYMBOL(bnf,i);
  --i->product_part;

  int j=1;
  bnf_index bi;
  for(;j<=bnf->items[current][0][1];++j)
  {  
    bi.item=current;
    bi.product=j;
    bi.product_part=0;
    sym>0?
      bi.lookaheads=bnf_get_first_set(bnf,sym):
      bi.lookaheads=i->lookaheads;

    if(!cset_is_member(closure,(void*)&bi))
    {
      slist_append(closure->members,new_bnf_index(bi.item,bi.product,bi.product_part,bi.lookaheads));
      bnf_kernel_items(bnf,&bi,closure);
    }
    else 
      printf("not added %d %d %d\n",bi.item,bi.product,bi.product_part);
  }
}

void bnf_closure(bnf_grammar* bnf,
                 bnf_index* i,
                 cset* closure)
{
  int next_right=bnf->items[i->item][i->product][i->product_part];

  /*no more products to be seen here*/
  if(next_right<0||next_right>=bnf->start_of_actions||IS_TERM(bnf,next_right))
    return;

  int sym=0;
  ++i->product_part;
  sym=PART_SYMBOL(bnf,i);
  --i->product_part;
    
  /*recurse through each product and add with lookaheads*/
  int j=1;  
  bnf_index bi;
  for(;j<=bnf->items[next_right][0][1];++j)
  {
    /*now attach lookaheads*/
    bi.item=next_right;
    bi.product=j;
    bi.product_part=0;
    sym>0?
      bi.lookaheads=bnf_get_first_set(bnf,sym):
      bi.lookaheads=i->lookaheads;

    /*try and add to the closure, if not augment its 
      generated lookaheads to the existing item in the set*/
    if(cset_is_member(closure,(void*)&bi))
      if(!add_lookaheads_to_item(closure,&bi))
        break;
      else
        bnf_closure(bnf,new_bnf_index(bi.item,bi.product,bi.product_part,bi.lookaheads),closure);
    else
    {
      slist_append(closure->members,new_bnf_index(bi.item,bi.product,bi.product_part,bi.lookaheads));
      bnf_closure(bnf,(bnf_index*)closure->members->_tail->_data,closure);
    }
      
  }
}

cset* bnf_get_item_kernels(cset* item)
{
  if(!item||!item->members)return 0;

  cset* kernels=new_set(compare_bnf_index);
  slist_elem* sle=item->members->_head;

  for(;sle;sle=sle->_next)
    if(((bnf_index*)sle->_data)->product_part>0)//any product not at 0 is a kernel
      cset_add(kernels,sle->_data);
    else if(((bnf_index*)sle->_data)->item==0&&//recognise a start symbol aswell
            ((bnf_index*)sle->_data)->product_part==0)
      cset_add(kernels,sle->_data);

  return kernels;
}

void bnf_goto_closure(bnf_grammar* bnf,
                      cset* item,
                      cset* closure_out,
                      int symbol)
{
  if(!bnf||!item||!closure_out)return;

  bnf_index* bi=0;
  slist_elem* sle=item->members->_head;
  for(;sle;sle=sle->_next)
  {
    bi=(bnf_index*)sle->_data;
    if(PART_SYMBOL(bnf,bi)==symbol)
    {
      //create the index, shifted by one and close and merge with current closure
      bi=new_bnf_index(bi->item,bi->product,bi->product_part+1,bi->lookaheads);
      cset_add(bi->lookaheads,(void*)(bnf->start_of_actions+1));
      cset_add(closure_out,(void*)bi);
      bnf_closure(bnf,bi,closure_out);
    }
  }
}

void bnf_merge_item_lookaheads(cset* merge,cset* b)
{
  slist_elem *mle=merge->members->_head,
             *ble=b->members->_head;
  int mcount=0;
  int bcount=0;
  for(;mle&&ble&&
       mcount<merge->members->_size&&
       bcount<b->members->_size;
       mle=mle->_next,ble=ble->_next,++mcount,++bcount)
    if(COMPARE_INDEX_PTRS(((bnf_index*)mle->_data),((bnf_index*)ble->_data)))
      ((bnf_index*)mle->_data)->lookaheads=
        cset_union_add(((bnf_index*)mle->_data)->lookaheads,
                       ((bnf_index*)ble->_data)->lookaheads);
}

bool propagate_lookaheads(bnf_grammar* bnf, cset* items)
{
  bnf_index* bi=0;
  bnf_index* bi2=0;
  cset* goto_set=0;
  slist_elem* sle=items->members->_head;
  slist_elem* sle2=0;
  slist_elem* sle3=0;
  slist_elem* sle4=0;
  int item_index=0;
  int goto_index=0;
  int sz_before=0;
  bool added=false;
  for(;sle;sle=sle->_next,++item_index)//each item
    for(sle2=((cset*)sle->_data)->members->_head;sle2;sle2=sle2->_next)//each product
      for(sle3=((bnf_index*)sle2->_data)->lookaheads->members->_head;sle3;sle3=sle3->_next)//each lookahead
        if((int)sle3->_data==bnf->start_of_actions+1)//this product propagates its kernel's lookaheads
        {
          goto_set=bnf_goto_set((bnf_index*)sle2->_data,items,0,&goto_index);
          //while(goto_set)
          //{
          if(goto_index!=-1)//goto_index>item_index&&<- taking this way seemed to help but compile times are LONG!!
          {
            //printf("PROPAGATOR(%d=>%d)\n",item_index,goto_index);
            for(sle4=goto_set->members->_head;sle4;sle4=sle4->_next)
            {
              bi=(bnf_index*)sle2->_data;
              bi2=(bnf_index*)sle4->_data;
              --bi2->product_part;
              if(COMPARE_INDEX_PTRS(bi,bi2))
              {
                //now we propagate this lookahead
                sz_before=bi2->lookaheads->members->_size;
                bi2->lookaheads=cset_union_add(bi2->lookaheads,bi->lookaheads);
                if(bi2->lookaheads->members->_size>sz_before)
                  added=true;
              }
              ++bi2->product_part;
            }
          }
            //break;
            //goto_set=bnf_goto_set((bnf_index*)sle2->_data,items,goto_index+1,&goto_index);
          //}
        }
  return added;
}

/*********************************************************
 * construct LR(1) items.                                *
 * This algorithm merges canonical LR(1) items together  *
 * based on thier common cores                           *
 *********************************************************/
cset* bnf_construct_items(bnf_grammar* bnf)
{
  cset* items=new_set(compare_bnf_item_sets);
  cset* current_set=new_set(compare_bnf_index);
  //cset current_set;
  //current_set.match_fn=compare_bnf_index;
  //current_set.members=new_slist();
  bnf_index* cbi=new_bnf_index(0,1,0,0);

  /*********************************************************
   * construct the first item/start state of the item set  *
   *********************************************************/
  cset_add(cbi->lookaheads,(void*)bnf->start_of_actions);
  cset_add(cbi->lookaheads,(void*)(bnf->start_of_actions+1));
  cset_add(current_set,(void*)cbi);
  bnf_closure(bnf,cbi,current_set);
  cset_add(items,(void*)current_set);

  int i=0;
  slist_elem* isle=items->members->_head;
  slist_elem* sle=0;
  int iind=0;
  int iadded=1;

  for(;isle;isle=isle->_next,++iind)
    /***********************************************************
    * cycle through all symbols and compute thier transitions *
    * for this current item by taking a goto closure and      *
    * trying to add it to the core item set.                  *
    * if we can't add then we merge lookaheads with a         *
    * matching state if any because we do not want to discard *
    * generated lookaheads.                                   *
    ***********************************************************/
    for(i=0;i<bnf->start_of_actions;++i)
    {
      bnf_goto_closure(bnf,(cset*)isle->_data,current_set,i);
      if(current_set&&current_set->members->_size>0)
      {
        if(!cset_add(items,(void*)current_set))
          bnf_merge_item_lookaheads((cset*)cset_get_first_match(items,current_set),current_set);

        current_set=new_set(compare_bnf_index);
      }
    }
  while(propagate_lookaheads(bnf,items));
  return items;
}

/*********************************************************
 * find the goto set of an index within a grammars items *
 *********************************************************/
cset* bnf_goto_set(bnf_index* bi,
                   cset* items,
                   int start,
                   int* index)
{
  int i=0;
  slist_elem* sle=items->members->_head;
  bnf_index bc;
  bc.item=bi->item;
  bc.product=bi->product;
  bc.product_part=bi->product_part+1;

  for(;sle&&i<start;sle=sle->_next,++i);
  for(;sle;sle=sle->_next,++i)
    if(cset_is_member((cset*)sle->_data,(void*)&bc))
    {
      if(index)
        *index=i;
      return (cset*)sle->_data;
    }
  if(index)*index=-1;
  return 0;
}

/************************************************************
 * this is the goto function that is used by the parse      *
 * table constructor. cycle through the passed item set,    *
 * match the index in the set and return its position index *
 ************************************************************/
int bnf_goto(bnf_index* bi,
             cset* items)
{
  int index=0;
  slist_elem* sle=items->members->_head;
  bnf_index* bc=new_bnf_index(bi->item,bi->product,bi->product_part+1,bi->lookaheads);
  for(;sle;sle=sle->_next,++index)
    if(cset_is_member((cset*)sle->_data,(void*)bc))
      return index;
  return -1;
}

/*********************************************************
 * find the index of this product in order of appearance *
 * in the main items array for the grammar               *
 *********************************************************/
int bnf_find_product_index(bnf_grammar* bnf, bnf_index* bi)
{
  int i=0;
  int p=0;
  int index=0;
  for(;i<bnf->item_count;++i)
    if(i==bi->item)
    {
      for(p=0;p<bnf->items[i][0][1]+1;++p)
        if(p==bi->product)
          return index+=p;
    }
    else index+=bnf->items[i][0][1];
  return 1;
}


int** bnf_construct_parse_table(bnf_grammar* bnf,
                                cset* items)
{
  /*allocate rows and columns*/
  int** ptable=(int**)calloc(items->members->_size,sizeof(int*));
  int i=0;
  int psymb=0;
  slist_elem* sle=items->members->_head;
  slist_elem* sle2=0;
  slist_elem* sle3=0;
  bnf_index* bi=0;
  for(;i<items->members->_size,sle;++i,sle=sle->_next)
  {
    /*set up and allocate for the row*/
    ptable[i]=(int*)calloc(bnf->start_of_actions+1,sizeof(int));
    memset((void*)ptable[i],0,bnf->start_of_actions+1*sizeof(int));
    sle2=((cset*)sle->_data)->members->_head;
    for(;sle2;sle2=sle2->_next)
    {
      bi=(bnf_index*)sle2->_data;
      psymb=PART_SYMBOL(bnf,bi);
      switch(psymb)
      {
        case -3:
        case -1:
        case -2:
          /*************************************
           * add a reduction action entry for  *
           * each lookahead in this indices    * 
           * lookaheads set.                   *
           *************************************/
          if(bi->item==0)
          {
            /* special case start symbol reduction*/
            if(ptable[i][bnf->start_of_actions])printf("sr\n");
            else ptable[i][bnf->start_of_actions]=items->members->_size+1;
          }
          else
            for(sle3=bi->lookaheads->members->_head;sle3;sle3=sle3->_next)
              if((int)sle3->_data>bnf->start_of_actions)
                continue;
              else if(ptable[i][(int)(sle3->_data)]>0)
              {
                /*************************************
                 * handle a shift/reduce error here. * 
                 * by default we shift like YACC.    *
                 *************************************/
                printf("shift/reduce %d %d = %d\n",i,(int)(sle3->_data),ptable[i][(int)(sle3->_data)]);
                //print_item(bnf,items,ptable[i][(int)(sle3->_data)],false);
                //ptable[i][(int)(sle3->_data)]=bnf_find_product_index(bnf,bi);
              }
              else if(ptable[i][(int)(sle3->_data)]<0)
              {
                /*handle a reduce/reduce error here*/
                printf("rr [item:%d][symbol:%d] = %d / %d\n",i,(int)(sle3->_data),ptable[i][(int)(sle3->_data)],-bnf_find_product_index(bnf,bi));
              }
              else 
                ptable[i][(int)(sle3->_data)]=-bnf_find_product_index(bnf,bi);
          break;
        default:
          /************************************
           * add a shift action on a terminal *
           ************************************/
          if(IS_TERM(bnf,PART_SYMBOL(bnf,bi)))
          {
            if(ptable[i][PART_SYMBOL(bnf,bi)]<0)
            {
              //printf("shift/reduce %d %d = %d\n",i,PART_SYMBOL(bnf,bi),ptable[i][PART_SYMBOL(bnf,bi)]);
              ptable[i][PART_SYMBOL(bnf,bi)]=bnf_goto(bi,items);
            }
            else ptable[i][PART_SYMBOL(bnf,bi)]=bnf_goto(bi,items);
          }  
          /*************************************
           * add a goto action on non-terminal *
           *************************************/
          else if(IS_NON_TERM(bnf,PART_SYMBOL(bnf,bi)))
          {
            if(ptable[i][PART_SYMBOL(bnf,bi)]!=0);
            else ptable[i][PART_SYMBOL(bnf,bi)]=bnf_goto(bi,items);
          }
          /************************************
           * handle an error here             *
           ************************************/
          else 
            printf("parse table construction error\n");
          break;
      }
    }
  }
  return ptable;
}

void print_parse_table(bnf_grammar* bnf, int** pt, int num_rows)
{
  int i=0;
  int j=0;
  for(;i<num_rows;++i,printf("\n"))
    for(j=0;j<=bnf->start_of_actions;++j)
      printf("%d ", pt[i][j]);
}

void print_item(bnf_grammar* bnf,cset* items,int index,bool numeric)
{
  slist_elem* isle=items->members->_head;
  slist_elem* sle=0;
  slist_elem* sle2=0;
  bnf_index* cbi=0;
  bnf_index* cbi2=0;
  int item_index=-1;
  while(isle)
  {
    ++item_index;
    if(item_index<index)
    {
      isle=isle->_next;
      continue;
    }
    else if(item_index>index)
      break;

    printf("item %d.\n",item_index);
    sle=((cset*)isle->_data)->members->_head;/*this is an existing item*/
    while(sle)/*loop through its product indices*/
    {
      cbi=(bnf_index*)sle->_data;
      if(numeric)
      {
        printf("\t%d:%d:%d(",cbi->item,cbi->product,cbi->product_part);
        print_set(cbi->lookaheads,"%d,");
        printf(")\n");
      }
      else
      {
        cbi2=new_bnf_index(cbi->item,cbi->product,0,cbi->lookaheads);
        printf("\t%s->",&bnf->lexicon[bnf->items[cbi2->item][0][0]][1]);
        for(;PART_SYMBOL(bnf,cbi2)>=0;++cbi2->product_part)
          if(cbi2->product_part==cbi->product_part)
            printf(" . %s:",&bnf->lexicon[PART_SYMBOL(bnf,cbi2)][1]);
          else printf("%s:",&bnf->lexicon[PART_SYMBOL(bnf,cbi2)][1]);
        printf("(");
        sle2=cbi->lookaheads->members->_head;
        while(sle2)
        {
          if((int)sle2->_data<bnf->start_of_actions)
            printf("%s,",&bnf->lexicon[(int)sle2->_data][1]);
          else if((int)sle2->_data==bnf->start_of_actions)
            printf("$,");
          else 
            printf("#,");

          sle2=sle2->_next;
        }
        printf(")\n");
      }
      sle=sle->_next;
    } 
  }
}
void print_items(bnf_grammar* bnf,cset* items,bool numeric)
{
  slist_elem* isle=items->members->_head;
  slist_elem* sle=0;
  slist_elem* sle2=0;
  bnf_index* cbi=0;
  bnf_index* cbi2=0;
  int item_index=-1;
  while(isle)
  {
    printf("item %d.\n",++item_index);
    sle=((cset*)isle->_data)->members->_head;/*this is an existing item*/
    while(sle)/*loop through its product indices*/
    {
      cbi=(bnf_index*)sle->_data;
      if(numeric)
      {
        printf("\t%d:%d:%d(",cbi->item,cbi->product,cbi->product_part);
        print_set(cbi->lookaheads,"%d,");
        printf(")\n");
      }
      else
      {
        cbi2=new_bnf_index(cbi->item,cbi->product,0,cbi->lookaheads);
        printf("\t%s->",&bnf->lexicon[bnf->items[cbi2->item][0][0]][1]);
        for(;PART_SYMBOL(bnf,cbi2)>=0;++cbi2->product_part)
          if(cbi2->product_part==cbi->product_part)
            printf(" . %s:",&bnf->lexicon[PART_SYMBOL(bnf,cbi2)][1]);
          else printf("%s:",&bnf->lexicon[PART_SYMBOL(bnf,cbi2)][1]);
        printf("(");
        sle2=cbi->lookaheads->members->_head;
        while(sle2)
        {
          if((int)sle2->_data<bnf->start_of_actions)
            printf("%s,",&bnf->lexicon[(int)sle2->_data][1]);
          else if((int)sle2->_data==bnf->start_of_actions)
            printf("$,");
          else 
            printf("#,");

          sle2=sle2->_next;
        }
        printf(")\n");
      }
      sle=sle->_next;
    } 
    isle=isle->_next;
  }
}
void print_item_kernels(bnf_grammar* bnf,cset* items,bool numeric)
{
  slist_elem* isle=items->members->_head;
  slist_elem* sle=0;
  slist_elem* sle2=0;
  bnf_index* cbi=0;
  bnf_index* cbi2=0;
  int item_index=0;
  while(isle)
  {
    printf("item %d.\n",++item_index);
    sle=bnf_get_item_kernels((cset*)isle->_data)->members->_head;/*this is an existing item*/
    while(sle)/*loop through its product indices*/
    {
      cbi=(bnf_index*)sle->_data;/*make the left shift here, close and create a new item set*/
      if(numeric)
      {
        printf("\t%d:%d:%d(",cbi->item,cbi->product,cbi->product_part);
        print_set(cbi->lookaheads,"%d,");
        printf(")\n");
      }
      else
      {
        cbi2=new_bnf_index(cbi->item,cbi->product,0,cbi->lookaheads);
        printf("\t%s->",&bnf->lexicon[bnf->items[cbi2->item][0][0]][1]);
        for(;PART_SYMBOL(bnf,cbi2)>=0;++cbi2->product_part)
          if(cbi2->product_part==cbi->product_part)
            printf(" . %s:",&bnf->lexicon[PART_SYMBOL(bnf,cbi2)][1]);
          else printf("%s:",&bnf->lexicon[PART_SYMBOL(bnf,cbi2)][1]);
        printf("(");
        sle2=cbi->lookaheads->members->_head;
        while(sle2)
        {
          if((int)sle2->_data<bnf->start_of_actions)
            printf("%s,",&bnf->lexicon[(int)sle2->_data][1]);
          else if((int)sle2->_data==bnf->start_of_actions)
            printf("$,");
          else 
            printf("£,");

          sle2=sle2->_next;
        }
        printf(")\n");
      }
      sle=sle->_next;
    } 
    isle=isle->_next;
  }
}
