#pragma once 
#include "stdafx.h"
#include "skeleton_gen.h"

const char* gnu_public_license=
"\
*************************************************************************\n\
 *  clrgen Parser Generator                                               *\n\
 *  http://www.jmgcode.co.uk                                              *\
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
 *************************************************************************\
";

/**************************************
 * determine which skeleton variable  *
 * a string is. If it doesn't match   *
 * any registered variables return -1.*
 * for help, this function takes an   *
 * iterator as a second argument to   *
 * let the caller know the string     *
 * length of the matched variable     *
 **************************************/
int lex_skeleton_var(char* var,
                     int* i)
{
  int match=0;
  int len=0;

  if(!var)
    return -1;

  if(*var=='P')
    if(strncmp(var,"PTABLE_ROW_DIM",14)==0)
    {
      match=SKV_PTABLE_ROW_DIM;
      len=14;
    }
    else if(strncmp(var,"PTABLE_COL_DIM",14)==0)
    {
      match=SKV_PTABLE_COL_DIM;
      len=14;
    }
    else if(strncmp(var,"PTABLE",6)==0)
    {
      match=SKV_PTABLE;
      len=6;
    }
    else//unknown var
      match=-1;
  else if(strncmp(var,"REDUCE_ACT",10)==0)
  {
    match=-1;
    len=10;
  }
  else if(strncmp(var,"ACCEPT_STATE",12)==0)
  {
    match=SKV_ACCEPT_STATE;
    len=12;
  }
  else if(strncmp(var,"GRAMMAR_NAME",12)==0)
  {
    match=SKV_GRAMMAR_NAME;
    len=12;
  }
  else//unknown var
    return match=-1;

  /* now return any match and append the varname length
     to the iterator if it exists*/
  if(match!=-1)
  {
    if(i)
      *i+=len;
    return match;
  }
  return -1;
}

int skeleton_generate(bnf_grammar* bnf,
                      skeleton_format* skfmt,
                      char* filename,
                      char* template_ptr,
                      int** ptbl,
                      int ptbl_row_dim,
                      int ptbl_col_dim,
                      int accept_state)
{
  FILE* out_file=0;
  char* sk_buff=0;
  int err=1;
  int last_pt=0;
  int i=0;

  if(!bnf||!skfmt||!filename||!template_ptr)
    return -1;

  /**************************************************
   * load the output file using the skfmt specified *
   * file name                                      *
   **************************************************/
  out_file=fopen(filename,"w+");
  sk_buff=template_ptr;
  
  /**********************************************
   * print the current license on all generated *
   * files. if the user hasn't provided comment *
   * formatting then tough shit!                *
   **********************************************/
  fprintf(out_file,"%s%s%s",skfmt->comment_start,
                            gnu_public_license,
                            skfmt->comment_end);

  /************************************************
   * copy the skeleton buffer to file and replace *
   * variable references with thier values.       *
   ************************************************/
  for(;i<strlen(sk_buff);++i)
    if(sk_buff[i]=='\\')
      ++i;
    else if(sk_buff[i]=='$')
    {
      /* copy up to this point into the out file*/
      fprintf(out_file,"%.*s",i-last_pt,&sk_buff[last_pt]);

      /* make sure this ain't the last symbol*/
      if(i+1==strlen(sk_buff)-1)
        {err=-1; break;}

      /* now determine the var name and generate its data*/
      ++i;
      switch(lex_skeleton_var(&sk_buff[i],&i))
      {
        case SKV_PTABLE_ROW_DIM:
          fprintf(out_file,"%d",ptbl_row_dim);
          break;
        case SKV_PTABLE_COL_DIM:
          fprintf(out_file,"%d",ptbl_col_dim);
          break;
        case SKV_PTABLE:
          if(!write_parse_table(out_file,ptbl,ptbl_row_dim,ptbl_col_dim,skfmt))
            {err=-1;break;}
          break;
          /*Reduce actions
            if(!write_reduction_handler(out_file,bnf,skfmt))            
            {err=-1;break;}*/
        case SKV_ACCEPT_STATE:
          fprintf(out_file,"%d",accept_state);
          break;
        case SKV_GRAMMAR_NAME:
          fprintf(out_file,"%s",bnf->grammar_name);
          break;
        case -1:
        default:
          err=-1;//error
          break;
      }
      last_pt=i;
    }

  /* print any of the remaining skeleton and free the skeleton buffer*/
  fprintf(out_file,"%.*s",i-last_pt,&sk_buff[last_pt]);
  free(sk_buff);
  return err;
}

/**********************************************
 * spit our the parse table using user format *
 * Nothing special here, just iterations and  *
 * file prints                                *
 **********************************************/
int write_parse_table(FILE* skf,
                      int** ptbl,
                      int ptbl_row_dim,
                      int ptbl_col_dim,
                      skeleton_format* skfmt)
{
  int r=0;
  int c=0;

  if(!skf||!ptbl||!skfmt)
    return -1;

  /******************************************
   * check that sufficient format has been  *
   * specified. if not return an error.     *
   ******************************************/
  if(!skfmt->tbl_start||
     !skfmt->tbl_row_start||
     !skfmt->tbl_entry_delim||
     !skfmt->tbl_row_end||
     !skfmt->tbl_end)
     return -1;

  /*********************************************
   * print the table in user format            *
   *********************************************/
  fprintf(skf,"%s",skfmt->tbl_start);
  for(;r<ptbl_row_dim-1;++r)
  {
    fprintf(skf,"%s",skfmt->tbl_row_start);
    for(c=0;c<ptbl_col_dim-1;++c)
      fprintf(skf,"%d%s",ptbl[r][c],skfmt->tbl_entry_delim);
    fprintf(skf,"%d",ptbl[r][c]);
    fprintf(skf,"%s",skfmt->tbl_row_end);
  }
  fprintf(skf,"%s",skfmt->tbl_row_start);
  for(c=0;c<ptbl_col_dim-1;++c)
    fprintf(skf,"%d%s",ptbl[r][c],skfmt->tbl_entry_delim);
  fprintf(skf,"%d",ptbl[r][c]);
  fprintf(skf,"%s",skfmt->tbl_end);
  
  return 1;
}

int write_action(FILE* skf,
                 char* action,
                 int prod_len,
                 skeleton_format* skfmt)
{
  int len=strlen(action?action:"");
  int last_start=0;
  int index=0;
  int i=0;

  if(!skf||!action||!skfmt)
    return -1;

  /**************************************
   * Print action start formatting      *
   **************************************/
   fprintf(skf,"%s",skfmt->action_start);

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
      fprintf(skf,"%.*s",i-last_start,&action[last_start]);

      /*********************************************
       * Here a reference to the item variable($)  *
       * is bieng made. just input the name of the *
       * global variable holding temporary values  *
       * for the current items.                    *
       *********************************************/
      if(i<len-1)
        if(!isdigit(action[i+1]))/*make sure this ref doesn't reference a part*/
          if(action[i+1]=='$')
          {
            fprintf(skf,"%s",skfmt->token_synth_val);
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
        fprintf(skf,"%s%d%s",skfmt->stack_access_start,
                             prod_len-index,
                             skfmt->stack_access_end);
      last_start=i;
    }

  /**************************************
   * print the remainder of the action  *
   * and then the action end formatting *
   **************************************/
  fprintf(skf,"%s%s",&action[last_start],skfmt->action_end);
  return 1;
}

int write_reduction_handler(FILE* skf,
                            bnf_grammar* bnf,
                            skeleton_format* skfmt)
{
  int i=1;
  int p=0;
  int pp=0;
 
  if(!skf||!bnf||!skfmt)
    return -1;

  /******************************************
   * check that sufficient format has been  *
   * specified. if not return an error.     *
   ******************************************/
  if(!skfmt->reduce_act_case_start||
     !skfmt->reduce_act_case_end||
     !skfmt->reduce_act_ret_start||
     !skfmt->reduce_act_ret_end||
     !skfmt->reduce_act_return_start||
     !skfmt->reduce_act_return_end)
     return -1;



  /******************************************
   * output the switch cases and actions    *
   * for each product.                      *
   ******************************************/
  for(;i<bnf->item_count;++i)
    for(p=1;p<=bnf->items[i][0][1];++p)
      for(pp=0;;++pp)
        if(bnf->items[i][p][pp]==-2)
        {
          fprintf(skf,"%s%d%s%s%d%s",
                  skfmt->reduce_act_case_start,
                  bnf_find_product_index(bnf,new_bnf_index(i,p,pp+1,0)),
                  skfmt->reduce_act_case_end,
                  skfmt->reduce_act_ret_start,
                  bnf->items[i][0][0],
                  skfmt->reduce_act_ret_end);
          if(!write_action(skf,&bnf->lexicon[bnf->items[i][p][pp+1]][1],pp,skfmt))
            return -1;
          fprintf(skf,"%s%d%s",
                  skfmt->reduce_act_return_start,
                  pp,
                  skfmt->reduce_act_return_end);
          break;
        }
        else if(bnf->items[i][p][pp]==-1)
        {
          fprintf(skf,"%s%d%s%s%d%s%s%d%s",
                  skfmt->reduce_act_case_start,
                  bnf_find_product_index(bnf,new_bnf_index(i,p,pp+1,0)),
                  skfmt->reduce_act_case_end,
                  skfmt->reduce_act_ret_start,
                  bnf->items[i][0][0],
                  skfmt->reduce_act_ret_end,
                  skfmt->reduce_act_return_start,
                  pp-1,//should this be -1?
                  skfmt->reduce_act_return_end);
          break;
        }
  return 1;
}
