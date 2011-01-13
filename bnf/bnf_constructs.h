#ifndef BNF_CONSTRUCTS_H
#define BNF_CONSTRUCTS_H

#pragma once
#include "stdafx.h"
#include "set.h"
/*#include "stack.h"*/

#define IS_TERM(bnf,a)(a>=bnf->start_of_terms&&a<bnf->start_of_actions)
#define IS_NON_TERM(bnf,a)(a<bnf->start_of_terms)
#define IS_ACTION(bnf,a)(a>=bnf->start_of_actions)
#define PART_SYMBOL(bnf,a)(bnf->items[a->item][a->product][a->product_part])
#define PART_SYMBOL_INT(bnf,i,p,pp)(bnf->items[i][p][pp])
#define COMPARE_INDEX(a,b)( a.item==b.item&&\
                            a.product==b.product&&\
                            a.product_part==b.product_part )
#define COMPARE_INDEX_PTRS(a,b)( a->item==b->item&&\
                                 a->product==b->product&&\
                                 a->product_part==b->product_part )
/******************************
 * code profiling definitions *
 ******************************/
static time_t prstart=0;
static time_t prend=0;
#define PROFILE_FN(name,funcall)( time(&prstart),funcall,time(&prend),\
                                  printf("function %s took %.2lf seconds.\n",name,difftime(prend,prstart)); )
#define PROFILE_START()( time(&prstart) )
#define PROFILE_STOP(name)( time(&prend),printf("%s took %.2lf seconds.\n",name,difftime(prend,prstart)) )


static cset** first_items=0;
static bool** added_array=0;

typedef struct _bnf_grammar
{
  int item_count;
  int start_of_terms;
  int start_of_actions;
  int symbol_count;
  char** lexicon;
  int*** items;

  char* grammar_name;
  char* author;
  unsigned char tbl_compression_type;
  char* skeleton_fpath;
  char* output_dir;
  char* log_file;
  unsigned char log_types;//verbose,none,console etcetc
  char* start_symbol;
  char* pre_includes;
  char* post_includes;

  
  int* precedence;
  int* associativity;

} bnf_grammar;

typedef struct _bnf_index
{
  int item;
  int product;
  int product_part;
  cset* lookaheads;
} bnf_index;

typedef struct _bnf_parse_item
{
  char* head;
  slist* products;
} bnf_parse_item;

int convert_to_bnfg(bnf_grammar* bnf,
                    slist* items,
                    cset* products,
                    cset* tokens,
                    cset* actions);

bnf_parse_item* new_bnf_parse_item(char* head);
bnf_grammar* new_bnf_grammar();
bnf_index* new_bnf_index(int i,
                         int p,
                         int pp,
                         cset* lookaheads);

bool compare_bnf_lookaheads(const void* a,void* b);
bool compare_bnf_index(const void* a,void* b);
bool compare_bnf_index_lookaheads(const void* a,void* b);
bool compare_bnf_item_sets(const void* a,void* b);
bool compare_bnf_item_kernels(const void* a, void* b);
bool compare_bnf_item_kernels_lookaheads(const void* a, void* b);

int** bnf_index_parse_item(bnf_grammar* bnf,
                           bnf_parse_item* item);
int bnf_index_of_symbol(bnf_grammar* bnf,
                        char* item);
int bnf_first(bnf_grammar* bnf,
              int symbol);
cset* bnf_get_first_set(bnf_grammar* bnf,
                        int symbol);
void bnf_closure(bnf_grammar* bnf,
                 bnf_index* i,
                 cset* closure);
void bnf_merge_item_lookaheads(cset* merge,
                               cset* b);
cset* bnf_get_item_kernels(cset* item);
void bnf_goto_closure(bnf_grammar* bnf,cset* item,cset* closure_out,int symbol);
int bnf_goto(bnf_index* bi,
             cset* items);
cset* bnf_goto_set(bnf_index* bi,
                   cset* items,
                   int start,
                   int* index);
bool propagate_lookaheads(bnf_grammar* bnf,
                          cset* items);
int bnf_find_product_index(bnf_grammar* bnf,
                           bnf_index* bi);
cset* bnf_construct_items(bnf_grammar* bnf);
void print_item(bnf_grammar* bnf,cset* items,int index,bool numeric);
void print_items(bnf_grammar* bnf,cset* items,bool numeric);
void print_item_kernels(bnf_grammar* bnf,cset* items,bool numeric);
void print_parse_table(bnf_grammar* bnf, int** pt, int num_rows);
int** bnf_construct_parse_table(bnf_grammar* bnf,
                                cset* items);

#endif