#pragma once
#include "stdafx.h"
#include "parser.h"
#include "lexer.h"
#include "set.h"


/*
item:T(HEAD)<body>T(SEMI_COLON){ printf(\"item:<head><body>T(SEMI_COLON)\\n\"); }\
    ;\
body:<product>{printf(\"body:<product>\\n\");}\
|<product>T(ACTION){printf(\"body:<product>T(ACTION)\\n\");}\
    |<body>T(PIPE)<body>{printf(\"body:<body>T(PIPE)<body>\\n\");}\
    ;\
product:<part>{printf(\"product:<part>\\n\");}\
|<product><part>{printf(\"product:<product><part>\\n\");}\
;\
part:T(TOKEN){printf(\"part:T(TOKEN)\\n\");}\
|T(PRODUCT){printf(\"part:T(PRODUCT)\\n\");}\
    ;\
*/

bool success=false;
bnf_parse_item* item=0;
static slist* product=new_slist();
static char* temp_val=0;
cset* products=new_set(comp_str);
cset* tokens=new_set(comp_str);
cset* actions=new_set(comp_str);

//beginning of stack data structure used by this parser
struct pstack_elem{void *_data;pstack_elem *_next;};
struct pcstack{size_t size;pstack_elem* first;};
pcstack* new_pcstack()
{
  pcstack* pcs((pcstack*)malloc(sizeof(pcstack)));
  memset((void*)pcs,0,sizeof(pcstack));
  pcs->size=0;
  pcs->first=0;
  return pcs;
}
void* peek_stack(pcstack* stack)
{
  return !stack||!stack->first?0:stack->first->_data;
}
void push_stack(pcstack *stack,void *data)
{
  if(!stack)return;
  pstack_elem* new_elem=(pstack_elem*)malloc(sizeof(pstack_elem));
  memset((void*)new_elem,0,sizeof(pstack_elem));
  new_elem->_data=data;
  if(!stack->first)
    new_elem->_next=0;
  else
    new_elem->_next=stack->first;
  (*stack).first=new_elem;
  (*stack).size+=1;
}
void* peek_stackn(pcstack* stack,
                  int index)
{
  int x=0;
  pstack_elem* curr_elem=stack->first;
  for(;x<=index;++x)
  {
    if(!curr_elem)return 0;
    if(x==index&&curr_elem)
      return curr_elem->_data;
    curr_elem=curr_elem->_next;
  }
  return 0;
}
void* pop_stack(pcstack *stack)
{
  if(!stack||!stack->first)return 0;
  void *ret=stack->first->_data;
  pstack_elem* temp=0;
  if(stack->first->_next)
  {
    temp=stack->first->_next;
    free((*stack).first);
    (*stack).first=temp;
    (*stack).size-=1;
  }
  else
  {
   free((*stack).first);
   (*stack).first=0;
   (*stack).size=0;
  }
  return ret;
}
void destroy_stack(pcstack* stack)
{
  pstack_elem *se_curr=stack->first,
              *se_next=0;
  while(se_curr&&se_curr->_next)
  {
    se_next=se_curr->_next;
    se_curr->_next=0;
    se_curr->_data=0;
    free(se_curr);
    se_curr=se_next;
  }
  (*stack).first=0;
  (*stack).size=-1;
}
  
//parser table declaration
int parse_table[15][13]=
{{0,1,2,0,0,0,0,3,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,16},
{0,0,0,4,5,6,0,7,0,0,0,8,0},
{0,0,0,0,0,0,0,0,9,0,0,0,0},
{0,0,0,0,0,0,10,0,0,0,11,0,0},
{0,0,0,0,0,12,-4,7,0,13,-4,8,0},
{0,0,0,0,0,0,-7,-7,0,-7,-7,-7,0},
{0,0,0,0,0,0,-10,-10,0,-10,-10,-10,0},
{0,0,0,0,0,0,-9,-9,0,-9,-9,-9,0},
{0,0,0,0,0,0,0,-3,0,0,0,-3,0},
{0,0,0,0,0,0,0,0,0,0,0,0,-2},
{0,0,0,14,5,6,0,7,0,0,0,8,0},
{0,0,0,0,0,0,-8,-8,0,-8,-8,-8,0},
{0,0,0,0,0,0,-5,0,0,0,-5,0,0},
{0,0,0,0,0,0,-6,0,0,0,11,0,0}};

//reduction actions switch function
int __prh(int* ri, pcstack* stack)
{
  switch(-*ri)
  {
    case 2:
      *ri=1;
      //printf("item:<head><body>T(SEMI_COLON)\n"); 
      return 3;
    case 3:
      *ri=2;
      //printf("head:T(PRODUCT)T(COLON)\n");
      temp_val=(char*)malloc(strlen(((lp_token*)peek_stackn(stack,1))->lex_val)+2);
      sprintf(temp_val,"<%s\0",((lp_token*)peek_stackn(stack,1))->lex_val);
      cset_add(products,(void*)temp_val);
      item=new_bnf_parse_item(temp_val);
      return 2;
    case 4:
      *ri=3;
      //printf("body:<product>\n");
      slist_append(item->products,(void*)product);
      product=new_slist();
      return 1;
    case 5:
      *ri=3;
      //printf("body:<product>T(ACTION)\n");
      temp_val=(char*)malloc(strlen(((lp_token*)peek_stackn(stack,0))->lex_val)+2);
      sprintf(temp_val,"{%s\0",((lp_token*)peek_stackn(stack,0))->lex_val);
      cset_add(actions,(void*)temp_val);
      slist_append(product,(void*)temp_val);
      slist_append(item->products,(void*)product);
      product=new_slist();
      return 2;
    case 6:
      *ri=3;
      //printf("body:<body>T(PIPE)<body>\n");
      return 3;
    case 7:
      *ri=4;
      //printf("product:<part>\n");
      return 1;
    case 8:
      *ri=4;
      //printf("product:<product><part>\n");
      return 2;
    case 9:
      *ri=5;
      //printf("part:T(TOKEN)\n");
      temp_val=(char*)malloc(strlen(((lp_token*)peek_stackn(stack,0))->lex_val)+2);
      sprintf(temp_val,"(%s\0",((lp_token*)peek_stackn(stack,0))->lex_val);
      cset_add(tokens,(void*)temp_val);
      slist_append(product,(void*)temp_val);
      return 1;
    case 10:
      *ri=5;
      //printf("part:T(PRODUCT)\n");
      temp_val=(char*)malloc(strlen(((lp_token*)peek_stackn(stack,0))->lex_val)+2);
      sprintf(temp_val,"<%s\0",((lp_token*)peek_stackn(stack,0))->lex_val);
      //cset_add(products,(void*)temp_val);
      slist_append(product,(void*)temp_val);
      return 1;
    default:
      return-1;
  }
}

//the parser function itself
lp_token* new_token(int lid, char* lval)
{
  lp_token* ret=(lp_token*)malloc(sizeof(lp_token));
  ret->lex_id=lid;
  if(lval)
  {
    int len=strlen(lval);
    ret->lex_val=(char*)malloc(len+1);
    ret->lex_val[len]='\0';
    strcpy(ret->lex_val,lval);
  }else ret->lex_val=0;
  return ret;
}

void __parser(lp_token** input,
              int count)
{
  success=false;
  int top_state=0;
  int input_index=0;
  int accept_state=16;
  int pop_count=0;
  int action=0;

  //initialize the stack at state 0
  pcstack* parse_stack=new_pcstack();
  push_stack(parse_stack,(void*)new_token(0,0));

  while(true)
  {
    top_state=((lp_token*)peek_stack(parse_stack))->lex_id;
    if(input_index==count)action=parse_table[top_state][12];
    else if(input[input_index]->lex_id>=12)return;
    else action=parse_table[top_state][input[input_index]->lex_id];

    if(action==accept_state)//accept
    {
      action=-((lp_token*)peek_stack(parse_stack))->lex_id;
      __prh(&action,parse_stack);
      //printf("accept\n");
      success=true;
      return;
    }
    if(action>0)//shift
    {
      //printf("shift\n");
      push_stack(parse_stack,(void*)new_token(action,input[input_index]->lex_val));
      ++input_index;
    }
    else if(action<0)//reduce
    {
      pop_count=__prh(&action,parse_stack);
      if(pop_count==-1)break;//catch errors here
      while(pop_count>0)
      {
        pop_stack(parse_stack);
        --pop_count;
      }
      push_stack(parse_stack,(void*)new_token(parse_table[((lp_token*)peek_stack(parse_stack))->lex_id][action],0));
      //printf("reduce\n");
    }
    else//error
    {
      //printf("error\n");
      return;
    }
  }
}

slist* parse_bnf(char* input)
{
  if(!input)return 0;

  slist* items=new_slist();
  slist* lexed_in=lex_bnf(input);
  lp_token** lex_arr=(lp_token**)calloc(lexed_in->_size,sizeof(lp_token*));
  int last_item=0;
  int i=0;
  slist_elem* sle=lexed_in->_head;

  for(;i<lexed_in->_size&&sle;++i,sle=sle->_next)
  {
    lex_arr[i]=(lp_token*)sle->_data;
    if(lex_arr[i]->lex_id==SEMI_COLON)
    {
      __parser(&lex_arr[last_item],(i-last_item)+1);
      last_item=i+1;
      if(success)
        slist_append(items,(void*)item);
      else return 0;
    }
  }
  free(lex_arr);
  slist_destroy(lexed_in);
  return items;
}
