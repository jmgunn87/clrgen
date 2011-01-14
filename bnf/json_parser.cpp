#pragma once
#include "stdafx.h"
#include "json_parser.h"
#include "utility.h"

json_value* new_json_value(int vt,void* data)
{
  json_value* jv=(json_value*)malloc(sizeof(json_value));
  jv->value_type=vt;
  jv->value=data;
  return jv;
}

json_pair* new_json_pair(char* k,json_value* v)
{
  json_pair* jp=(json_pair*)malloc(sizeof(json_pair));
  jp->key=k;
  jp->value=v;
  return jp;
}

slist* objects_root=new_slist();
slist* current_object=new_slist();
slist* current_array=new_slist();
json_pair* current_pair=0;

void __json_clear_temp()
{

  /***********************************************
   * clear lists and pairs then reallocate them  *
   ***********************************************/
  slist_destroy(current_array);
  slist_destroy(objects_root);
  slist_destroy(current_object);
  free(current_pair);

  objects_root=new_slist();
  current_object=new_slist();
  current_array=new_slist();

}

lp_token* new_token(int lid, void* lval)
{
  lp_token* ret=(lp_token*)malloc(sizeof(lp_token));
  ret->lex_id=lid;
  ret->lex_val=lval;
  return ret;
}

/**********************************************
 * Parser configurations                      *
 *--------------------------------------------*
 **********************************************/
#define ACCEPT 27
void* __item_synth_val=0;

/**********************************************
 * define our memory allocation functions     *
 *--------------------------------------------*
 * This will be either a user defined pointer *
 * or by default clib memory allocators.      *
 **********************************************/
static void*(*allocator)(size_t)=malloc;
static void*(*re_allocator)(void*,size_t)=realloc;
static void(*de_allocator)(void*)=free;

/****************************************************
 * declare, define and initialize the parser stack  *
 * and global macro functions for use by the parser *
 *--------------------------------------------------*
 ****************************************************/
#define STACK_INIT_SIZE 50
#define STACK_GROW_AMOUNT 10
int stack_cursor=-1;
static int stack_size=STACK_INIT_SIZE;
/*declare and intialize the stack*/
lp_token* stack=(lp_token*)allocator(STACK_INIT_SIZE*sizeof(lp_token));

/*stack macros*/
#define EXTEND_STACK()\
{\
  stack_size+=STACK_GROW_AMOUNT;\
  stack=(lp_token*)re_allocator(stack,stack_size*sizeof(lp_token));\
}
#define PUSH_STACK(id,val)\
{\
  if(stack_cursor+1>=stack_size-1)\
    EXTEND_STACK();\
  stack[stack_cursor+1].lex_id=id;\
  stack[stack_cursor+1].lex_val=val;\
  ++stack_cursor;\
}
#define POP_STACK(n)( stack_cursor-=n )
#define PEEK_STACK(n)( stack[stack_cursor-n] )

static int parse_table[26][19]=
{{0,1,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,27},
{0,0,3,4,0,0,0,0,5,0,6,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,7,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,-4,8,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,-2,-2,0,0,0,-2,0,0,0,0,-2},
{0,0,0,0,0,0,0,0,0,0,0,9,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,-3,-3,0,0,0,-3,0,0,0,0,-3},
{0,0,10,4,0,0,0,0,0,0,6,0,0,0,0,0,0,0,0},
{0,11,0,0,12,0,13,2,0,0,14,0,15,0,16,17,18,19,0},
{0,0,0,0,0,0,0,0,-5,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,-13,-13,0,0,0,-13,0,0,0,0,0},
{0,0,0,0,0,0,0,0,-14,-14,0,0,0,-14,0,0,0,0,0},
{0,0,0,0,0,0,0,0,-6,-6,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,-11,-11,0,0,0,-11,0,0,0,0,0},
{0,11,0,0,12,20,21,2,0,0,14,0,15,22,16,17,18,19,0},
{0,0,0,0,0,0,0,0,-12,-12,0,0,0,-12,0,0,0,0,0},
{0,0,0,0,0,0,0,0,-15,-15,0,0,0,-15,0,0,0,0,0},
{0,0,0,0,0,0,0,0,-16,-16,0,0,0,-16,0,0,0,0,0},
{0,0,0,0,0,0,0,0,-17,-17,0,0,0,-17,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,23,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,24,0,0,0,-9,0,0,0,0,0},
{0,0,0,0,0,0,0,0,-7,-7,0,0,0,-7,0,0,0,0,0},
{0,0,0,0,0,0,0,0,-8,-8,0,0,0,-8,0,0,0,0,0},
{0,11,0,0,12,25,21,2,0,0,14,0,15,0,16,17,18,19,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,-10,0,0,0,0,0}};

//reduction actions switch function
static int __prh(int* ri)
{
  switch(-*ri)
  {
    case 2:
      *ri=1;
      printf("<object>\n");
      return 2;
    case 3:
      *ri=1;
      
    printf("<object>\n");
    __item_synth_val=(void*)current_object;
    current_object=new_slist();
    
  
      return 3;
    case 4:
      *ri=2;
      
    printf("<members>\n");
    slist_append(current_object,PEEK_STACK(0).lex_val);
  
      return 1;
    case 5:
      *ri=2;
      
    printf("<members>\n");
    slist_append(current_object,PEEK_STACK(2).lex_val);
  
      return 3;
    case 6:
      *ri=3;
      
    printf("<pair>\n");
    current_pair=new_json_pair((char*)PEEK_STACK(2).lex_val,(json_value*)PEEK_STACK(0).lex_val);
    __item_synth_val=(void*)current_pair;
  
      return 3;
    case 7:
      *ri=4;
      printf("<array>\n");
      return 2;
    case 8:
      *ri=4;
      
    printf("<array>\n");
    __item_synth_val=(void*)current_array;
    current_array=new_slist();
  
      return 3;
    case 9:
      *ri=5;
      
    printf("<elements>\n");
    slist_append(current_array,(void*)PEEK_STACK(0).lex_val);
    __item_synth_val=PEEK_STACK(0).lex_val;
  
      return 1;
    case 10:
      *ri=5;
      
    printf("<elements>");
    slist_append(current_array,(void*)PEEK_STACK(2).lex_val);
    __item_synth_val=PEEK_STACK(2).lex_val;
  
      return 3;
    case 11:
      *ri=6;
       printf("<value>\n"); __item_synth_val=(void*)new_json_value(JS_STRING,PEEK_STACK(0).lex_val);
      return 1;
    case 12:
      *ri=6;
       printf("<value>\n"); __item_synth_val=(void*)new_json_value(JS_NUMBER,PEEK_STACK(0).lex_val);
      return 1;
    case 13:
      *ri=6;
       printf("<value>\n"); __item_synth_val=(void*)new_json_value(JS_OBJECT,PEEK_STACK(0).lex_val);
      return 1;
    case 14:
      *ri=6;
       printf("<value>\n"); __item_synth_val=(void*)new_json_value(JS_ARRAY,PEEK_STACK(0).lex_val); 
      return 1;
    case 15:
      *ri=6;
       printf("<value>\n"); __item_synth_val=(void*)new_json_value(JS_TRUE,PEEK_STACK(0).lex_val);  
      return 1;
    case 16:
      *ri=6;
       printf("<value>\n"); __item_synth_val=(void*)new_json_value(JS_FALSE,PEEK_STACK(0).lex_val); 
      return 1;
    case 17:
      *ri=6;
       printf("<value>\n"); __item_synth_val=(void*)new_json_value(JS_NULL,PEEK_STACK(0).lex_val);  
      return 1;
    default:
      return-1;
  }
}

/*the parser function itself*/
int __json_parse_token(lp_token* input)
{
  int top_state=0;
  int pop_count=0;
  int action=0;
  
  /*initialize the stack at state 0*/
  if(stack_cursor==-1)
    PUSH_STACK(0,0);
  
  while(1)
  {
    top_state=PEEK_STACK(0).lex_id;
    if(!input)action=parse_table[top_state][18];
    else if(input->lex_id>=18)return 0;
    else action=parse_table[top_state][input->lex_id];
    
    if(action==ACCEPT)/*ACCEPT*/
    {
      action=--(PEEK_STACK(0).lex_id);
      __prh(&action);
      return ACCEPT;
    }
    if(action>0)/*SHIFT*/
    {
      PUSH_STACK(action,input->lex_val);
      return 1;
    }
    else if(action<0)/*REDUCE*/
    {
      pop_count=__prh(&action);
      if(pop_count==-1)
        return 0;/*catch errors here*/
      POP_STACK(pop_count);/*pop the stack n times specified by __prh*/
      PUSH_STACK(parse_table[PEEK_STACK(0).lex_id][action],__item_synth_val);/*push the reduction along with any synthesised values*/
      __item_synth_val=0;
      continue;
    }
    else/*error*/
    {
      return 0;
    }
  }
}

/********************************************
 * Lex a .jy file completely. this is an    *
 * all-in-one lexer, and makes exception    *
 * for <jsk$ & <jy$ tags which help protect *
 * quote issues when strings encoded in the *
 * user sections contain quotes.            *
 ********************************************/
slist* __json_lex(char* str)
{
  int i=0;
  int sublen=0;
  int lookahead=0;
  char* substr=0;
  slist* lexed_inputs=0;
    
  if(!str)
    return 0;

  lexed_inputs=new_slist();
  for(;i<strlen(str);++i)
    switch(str[i])
    {
      case ' ':case '\t':
        break;
      case '\\':
        ++i;
        break;
      case '#':
        for(;i<strlen(str),str[i]!='\n';++i);
        break;
      case '{':
        slist_append(lexed_inputs,(void*)new_token(JS_LBRACE,0));
        break;
      case '}':
        slist_append(lexed_inputs,(void*)new_token(JS_RBRACE,0));
        break;
      case '[':
        slist_append(lexed_inputs,(void*)new_token(JS_LBRACKET,0));
        break;
      case ']':
        slist_append(lexed_inputs,(void*)new_token(JS_RBRACKET,0));
        break;      
      case '\"':
        for(sublen=0;i<strlen(str)&&str[++i]!='\"';++sublen)
          if(str[i]=='\\')
            if(str[i+1]=='\"')
            {
              str[i]=' '; 
              ++i;
              ++sublen;
            }
            else if(str[i+1]=='n')
            {
              str[i]=' ';
              str[i+1]='\n';
              ++i;
              ++sublen;
            }
        substr=(char*)malloc(sublen+1);
        substr[sublen]='\0';
        strncpy(substr,&str[i-sublen],sublen);
        slist_append(lexed_inputs,(void*)new_token(JS_STRING,substr));
        break;
      case ':':
        printf("lexed: :\n");
        slist_append(lexed_inputs,(void*)new_token(JS_COLON,0));
        break;
      case ',':
        printf("lexed:,\n");
        slist_append(lexed_inputs,(void*)new_token(JS_COMMA,0));
        break;
      case 't':
        if(strncmp(&str[i],"true",4)==0)
        {
          slist_append(lexed_inputs,(void*)new_token(JS_TRUE,0));
          i+=3;
        }
        break;
      case 'f':
        if(strncmp(&str[i],"false",5)==0)
        {
          slist_append(lexed_inputs,(void*)new_token(JS_FALSE,0));
          i+=4;
        }
        break;
      case 'n':
        if(strncmp(&str[i],"null",4)==0)
        {
          slist_append(lexed_inputs,(void*)new_token(JS_NULL,0));
          i+=3;
        }
        break;
      default:
        if(isdigit(str[i]))//handle a number
        {
          slist_append(lexed_inputs,(void*)new_token(JS_NUMBER,(void*)atoi(&str[i])));
          while(isdigit(str[++i]));
        }
        else//format error
          ;
        break;
    }
  return lexed_inputs;
}

slist* json_decode(char* filepath)
{
  slist* json_tokens=0;
  slist_elem* sle=0;
  char* buffer=0;

  /**************************************
   * load the file into mem if existing *
   **************************************/
  buffer=file_load(filepath);
  if(!buffer)
    return 0;

  /**********************************
   * lex all input into an slist    *
   **********************************/
  json_tokens=__json_lex(buffer);
  if(!json_tokens||!json_tokens->_size)
    return 0;

  /*******************************
   * Now Parse all lexed inputs  *
   *******************************/
  sle=json_tokens->_head;
  while(sle)
    if(__json_parse_token((lp_token*)sle->_data)==0)
    {
      slist_destroy(json_tokens);
      __json_clear_temp();
      return 0;
    }
    else sle=sle->_next;
  if(__json_parse_token(0)==0)
  {
    slist_destroy(json_tokens);
    __json_clear_temp();
    return 0;
  }

  /*******************************
   * return the top of the stack *
   * which will contain the root *
   * object.                     *
   *******************************/
  else return (slist*)stack[stack_cursor].lex_val;
}
