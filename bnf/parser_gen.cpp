#include "stdafx.h"
#include "parser_gen.h"

#define ITEM_SYNTH_VALUE_NAME "__item_synth_val"
#define DEFAULT_PARSE_TABLE_NAME "parse_table"
#define DEFAULT_PARSER_NAME "__parse_token"
#define DEFAULT_STACK_INIT 50
#define DEFAULT_STACK_GROW 10
#define DEFAULT_ALLOCATOR "malloc"
#define DEFAULT_REALLOCATOR "realloc"
#define DEFAULT_DEALLOCATOR "free"

const char* gnu_license="\
/**************************************************************************\n\
 *  BISON-KILLER LR(1) Parser Generator                                   *\n\
 *  Copyright (C) 2011  James Marshall-Gunn                               *\n\
 *------------------------------------------------------------------------*\n\
 *  This program is free software: you can redistribute it and/or modify  *\n\
 *  it under the terms of the GNU General Public License as published by  *\n\
 *  the Free Software Foundation, either version 3 of the License, or     *\n\
 *  (at your option) any later version.                                   *\n\
 *                                                                        *\n\
 *  This program is distributed in the hope that it will be useful,       *\n\
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of        *\n\
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *\n\
 *  GNU General Public License for more details.                          *\n\
 *                                                                        *\n\
 *  You should have received a copy of the GNU General Public License     *\n\
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>  *\n\
 **************************************************************************/\n\
\n\
";
const char* token_def="\
lp_token* new_token(int lid, void* lval)\n\
{\n\
  lp_token* ret=(lp_token*)malloc(sizeof(lp_token));\n\
  ret->lex_id=lid;\n\
  ret->lex_val=lval;\n\
  return ret;\n\
}\n\
\n\
";

/**********************************************
 * This template takes a parser function name *
 * in string format.                          *
 *--------------------------------------------*
 **********************************************/
const char* header="\
\n\n\
#ifndef LPTOKENDECL_H\n\
#define LPTOKENDECL_H\n\n\
typedef struct lp_token\n\
{\n\
  int lex_id;\n\
  void* lex_val;\n\
}lp_token;\n\
\n\
lp_token* new_token(int lid,void* lval);\n\
\n\
#endif\n\n\
int %s(lp_token* input);\n\
\n\
";

/*********************************************
 * This template takes the names in decimal  *
 * format of the parser accept state and in  *
 * string format the item synth value name   *
 *-------------------------------------------*
 *********************************************/
const char* parser_config="\
/**********************************************\n\
 * Parser configurations                      *\n\
 *--------------------------------------------*\n\
 **********************************************/\n\
#define ACCEPT %d\n\
static void* %s=0;\n\
\n\
";

/*********************************************
 * This template takes the names in string   *
 * format of (a) a memory allocator          *
 *          (b) a memory re-allocator        *
 *          (c) a memory de-allocator        *
 *-------------------------------------------*
 *********************************************/
const char* allocators="\
/**********************************************\n\
 * define our memory allocation functions     *\n\
 *--------------------------------------------*\n\
 * This will be either a user defined pointer *\n\
 * or by default clib memory allocators.      *\n\
 **********************************************/\n\
static void*(*allocator)(size_t)=%s;\n\
static void*(*re_allocator)(void*,size_t)=%s;\n\
static void(*de_allocator)(void*)=%s;\n\
\n\
";

/*********************************************
 * This template takes an inital stack size  *
 * and a stack growth amount both specified  *
 * in integer format.                        *
 *-------------------------------------------*
 *********************************************/
const char* stack_source="\
/****************************************************\n\
 * declare, define and initialize the parser stack  *\n\
 * and global macro functions for use by the parser *\n\
 *--------------------------------------------------*\n\
 ****************************************************/\n\
#define STACK_INIT_SIZE %d\n\
#define STACK_GROW_AMOUNT %d\n\
static int stack_cursor=-1;\n\
static int stack_size=STACK_INIT_SIZE;\n\
/*declare and intialize the stack*/\n\
static lp_token* stack=(lp_token*)allocator(STACK_INIT_SIZE*sizeof(lp_token));\n\
\n\
/*stack macros*/\n\
#define EXTEND_STACK()\\\n\
{\\\n\
  stack_size+=STACK_GROW_AMOUNT;\\\n\
  stack=(lp_token*)re_allocator(stack,stack_size*sizeof(lp_token));\\\n\
}\n\
#define PUSH_STACK(id,val)\\\n\
{\\\n\
  if(stack_cursor+1>=stack_size-1)\\\n\
    EXTEND_STACK();\\\n\
  stack[stack_cursor+1].lex_id=id;\\\n\
  stack[stack_cursor+1].lex_val=val;\\\n\
  ++stack_cursor;\\\n\
}\n\
#define POP_STACK(n)( stack_cursor-=n )\n\
#define PEEK_STACK(n)( stack[stack_cursor-n] )\n\
\n\
";

/************************************************
 * This template takes a parser function name   *
 * in string format, Then the item synth global *
 * name twice.                                  *
 *----------------------------------------------*
 ************************************************/
const char* stepped_parser="\
int %s(lp_token* input)\n\
{\n\
  int top_state=0;\n\
  int pop_count=0;\n\
  int action=0;\n\
  \n\
  /*initialize the stack at state 0*/\n\
  if(stack_cursor==-1)\n\
    PUSH_STACK(0,0);\n\
  \n\
  while(1)\n\
  {\n\
    top_state=PEEK_STACK(0).lex_id;\n\
    if(!input)action=parse_table[top_state][%d];\n\
    else if(input->lex_id>=%d)return 0;\n\
    else action=parse_table[top_state][input->lex_id];\n\
    \n\
    if(action==ACCEPT)/*ACCEPT*/\n\
    {\n\
      action=--(PEEK_STACK(0).lex_id);\n\
      __prh(&action);\n\
      return ACCEPT;\n\
    }\n\
    if(action>0)/*SHIFT*/\n\
    {\n\
      PUSH_STACK(action,input->lex_val);\n\
      return 1;\n\
    }\n\
    else if(action<0)/*REDUCE*/\n\
    {\n\
      pop_count=__prh(&action);\n\
      if(pop_count==-1)\n\
        return 0;/*catch errors here*/\n\
      POP_STACK(pop_count);/*pop the stack n times specified by __prh*/\n\
      PUSH_STACK(parse_table[PEEK_STACK(0).lex_id][action],%s);/*push the reduction along with any synthesised values*/\n\
      %s=0;\n\
      continue;\n\
    }\n\
    else/*error*/\n\
    {\n\
      return 0;\n\
    }\n\
  }\n\
}\n\
";

void write_header(FILE* f,
                  bnf_grammar* bnf,
                  char* parser_fn_name)
{
  int i=bnf->start_of_terms;

  if(!f||!bnf)
    return;

  /* define each token */
  for(;i<bnf->start_of_actions;++i)
    fprintf(f,"#define %s %d\n",&bnf->lexicon[i][1],i);
  /* write the token structure declaration and parser fn*/
  fprintf(f,header,DEFAULT_PARSER_NAME);
}

void write_parser(bnf_grammar* bnf,
                  int** parse_table,
                  int rows,
                  int cols)
{
  if(!bnf||!parse_table)
    return;

  FILE* fh=fopen("__parser.h", "w+");
  fprintf(fh,gnu_license);
  write_header(fh,bnf,DEFAULT_PARSER_NAME);
  fclose(fh);

  int i=bnf->start_of_actions;
  FILE* f=fopen("__parser.c", "w+");
  fprintf(f,gnu_license);
  if(bnf->pre_includes!=0)
    fprintf(f,"%s\n\n",bnf->pre_includes);
  fprintf(f,token_def);
  fprintf(f,parser_config,rows+1,ITEM_SYNTH_VALUE_NAME);
  fprintf(f,allocators,DEFAULT_ALLOCATOR,DEFAULT_REALLOCATOR,DEFAULT_DEALLOCATOR);  
  write_stack_source(f,50,10);
  write_parse_table(f,parse_table,rows,cols,DEFAULT_PARSE_TABLE_NAME);
  fprintf(f,"\n\n//reduction actions switch function\n");
  write_reduction_handler(f,bnf);
  fprintf(f,"\n//the parser function itself\n");
  write_parser_fn(f,bnf,DEFAULT_PARSER_NAME,DEFAULT_PARSE_TABLE_NAME,rows+1);
  fclose(f);
}

/*************************************************
 * write our stack source code to the given file *
 *************************************************/
void write_stack_source(FILE* f,
                        unsigned long initial_size,
                        unsigned long growth_amount)
{
  if(!f)
    return;
  fprintf(f,stack_source,initial_size,growth_amount);
}

/**********************************************
 * spit our the parse table in c array form.  *
 * Nothing special here, just iterations and  *
 * file prints                                *
 **********************************************/
void write_parse_table(FILE* f,
                       int** table,
                       int rows,
                       int cols,
                       char* table_name)
{
  int r=0;
  int c=0;

  if(!f||!table||!table_name)
    return;

  fprintf(f,"static int %s[%d][%d]=\n{",table_name,rows,cols);
  for(;r<rows-1;++r)
  {
    fprintf(f,"{");
    for(c=0;c<cols-1;++c)
      fprintf(f,"%d,",table[r][c]);
    fprintf(f,"%d",table[r][c]);
    fprintf(f,"},\n");
  }
  fprintf(f,"{");
  for(c=0;c<cols-1;++c)
    fprintf(f,"%d,",table[r][c]);
  fprintf(f,"%d",table[r][c]);
  fprintf(f,"}};");
}

void write_action(FILE* f,
                  char* action,
                  int prod_len)
{
  int len=strlen(action?action:"");
  int last_start=0;
  int index=0;
  int i=0;

  if(!f||!action)return;

  /***************************************
   * loop through the action and replace *
   * any part references('$') with stack *
   * accessing code.                     *
   ***************************************/
  for(;i<len;++i)
    if(action[i]=='\\'&&i<len-1)
      ++i;
    else if(action[i]=='$')
    {
      /*print any previous input before this character*/
      fprintf(f,"%.*s",i-last_start,&action[last_start]);

      /*********************************************
       * Here a reference to the item variable($)  *
       * is bieng made. just input the name of the *
       * global variable holding temporary values  *
       * for the current items.                    *
       *********************************************/
      if(i<len-1)
        if(!isdigit(action[i+1]))/*make sure this ref doesn't reference a part*/
        {
          fprintf(f,ITEM_SYNTH_VALUE_NAME);
          last_start=i+1;
          continue;
        }

      /*********************************************
       * handle an item part reference ($N).       *
       * calulate the stack position for the item  *
       * and return reference from the stack       *
       *********************************************/
      if(i<len-1)/*get the stack index*/
        index=atoi(&action[i+1]);
      else index=0;
      while(isdigit(action[++i]));/*fast forward past all atoi'd characters*/
      /*print out the stack reference*/
      if(index>0)
        fprintf(f,"PEEK_STACK(%d).lex_val",prod_len-index);
      last_start=i;
    }

  /*************************************
   * print the remainder of the action *
   *************************************/
  fprintf(f,"%s",&action[last_start]);
}
void write_reduction_handler(FILE* f,
                             bnf_grammar* bnf)
{
  if(!f||!bnf)return;
  int i=1;
  int p=0;
  int pp=0;
  
  fprintf(f,"static int __prh(int* ri)\n{\n  switch(-*ri)\n  {\n");
  /******************************************
   * output the switch cases and actions    *
   * for each product.                      *
   ******************************************/
  for(;i<bnf->item_count;++i)
    for(p=1;p<=bnf->items[i][0][1];++p)
      for(pp=0;;++pp)
        if(bnf->items[i][p][pp]==-2)
        {
          fprintf(f,"    case %d:\n      *ri=%d;\n      ",
                  bnf_find_product_index(bnf,new_bnf_index(i,p,pp+1,0)),
                  bnf->items[i][0][0]);
          write_action(f,&bnf->lexicon[bnf->items[i][p][pp+1]][1],pp);
          fprintf(f,"\n      return %d;\n",pp);
          break;
        }
        else if(bnf->items[i][p][pp]==-1)
        {
          fprintf(f,"    case %d:*ri=%d;return %d;\n",
                  bnf_find_product_index(bnf,new_bnf_index(i,p,pp+1,0)),
                  bnf->items[i][0][0],pp-1);
          break;
        }

  /**************************************
   * now close the function declaration *
   **************************************/
  fprintf(f,"    default:\n      return-1;\n  }\n}\n");
}

void write_parser_fn(FILE* f,
                     bnf_grammar* bnf,
                     char* function_name,
                     char* table_name,
                     int accept_symbol)
{
  if(!f||!table_name)
    return;

  fprintf(f,
          stepped_parser,/*parser skeleton*/
          function_name,/*function name*/
          bnf->start_of_actions,/*accept symbol*/
          bnf->start_of_actions,/*null symbol*/
          ITEM_SYNTH_VALUE_NAME,
          ITEM_SYNTH_VALUE_NAME
         );
}


