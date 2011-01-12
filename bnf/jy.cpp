#pragma once
#include "stdafx.h"
#include "jy.h"

slist* parse_items=0;
extern cset* products;
extern cset* tokens;
extern cset* actions;

TBLCMPR_TYPE get_table_compression_type(char type)
{
  if(type=='N')
    return TBLCMPR_NONE;
  else if(type=='C')
    return TBLCMPR_PAIR_DUPLICATES;
  else return TBLCMPR_UNKNOWN;
};

LOGTYPE get_log_type(char type)
{
  if(type=='N')
    return LT_NONE;
  else if(type=='C')
    return LT_CONSOLE;
  else if(type=='L')
    return LT_LOGFILE;
  else if(type=='A')
    return LT_ALL;
  else return LT_UNKNOWN;
}

JSON_BNF_KEY get_key_type(char* key)
{
  if(!key)
    return KEY_UNKNOWN;

  if(strcmp(key,"grammar_name")==0)
    return KEY_GRAMMAR_NAME;
  else if(strcmp(key,"author")==0)
    return KEY_AUTHOR;
  else if(strcmp(key,"table_compression")==0)
    return KEY_COMPRESSION;
  else if(strcmp(key,"skeleton_filepath")==0)
    return KEY_SKPATH;
  else if(strcmp(key,"output_directory")==0)
    return KEY_OUT_DIR;
  else if(strcmp(key,"log_filename")==0)
    return KEY_LOG_NAME;
  else if(strcmp(key,"log_outputs")==0)
    return KEY_LOG_TYPES;
  else if(strcmp(key,"action_options")==0)
    return KEY_ACTION_OPTS;
  else if(strcmp(key,"default_sr_conflict")==0)
    return KEY_CONFLICTS;
  else if(strcmp(key,"tokens")==0)
    return KEY_TOKENS;
  else if(strcmp(key,"start_symbol")==0)
    return KEY_START_SYM;
  else if(strcmp(key,"pre_includes")==0)
    return KEY_PRE_INCLUDES;
  else if(strcmp(key,"post_includes")==0)
    return KEY_POST_INCLUDES;
  else if(strcmp(key,"grammar_def")==0)
    return KEY_GRAMMAR_DEF;
  else
    return KEY_UNKNOWN;
}

int json_to_bnf(slist* root,
                bnf_grammar* bnf)
{
  slist_elem* sle=0;
  slist_elem* sle_array=0;
  json_value* arr_val=0;
  LOGTYPE log_ty_temp;
  json_pair* cpair=0;

  if(!root||!root->_size||!bnf)
    return -1;

  sle=root->_head;
  while(sle)
  {
    if(sle->_data)
    {
      cpair=(json_pair*)sle->_data;
      switch(get_key_type(cpair->key))
      {
        case KEY_GRAMMAR_NAME:
          if(cpair->value->value_type!=JS_STRING)
            return -1;
          (*bnf).grammar_name=(char*)cpair->value->value;
          break;
        case KEY_AUTHOR:
          if(cpair->value->value_type!=JS_STRING)
            return -1;
          (*bnf).author=(char*)cpair->value->value;
          break;
        case KEY_COMPRESSION:
          if(cpair->value->value_type!=JS_STRING)
            return -1;
          (*bnf).tbl_compression_type=get_table_compression_type(*(char*)cpair->value->value);
          break;
        case KEY_SKPATH:
          if(cpair->value->value_type!=JS_STRING)
            return -1;
          (*bnf).skeleton_fpath=(char*)cpair->value->value;
          break;
        case KEY_OUT_DIR:
          if(cpair->value->value_type!=JS_STRING)
            return -1;
          (*bnf).output_dir=(char*)cpair->value->value;
          break;
        case KEY_LOG_NAME:
          if(cpair->value->value_type!=JS_STRING)
            return -1;
          (*bnf).log_file=(char*)cpair->value->value;
          break;
        case KEY_LOG_TYPES:
          if(cpair->value->value_type!=JS_ARRAY)
            return -1;
          sle_array=((slist*)cpair->value->value)->_head;
          while(sle_array)
          {
            arr_val=(json_value*)sle_array->_data;
            if(arr_val->value_type!=JS_STRING)
              return -1;

            log_ty_temp=get_log_type(*(char*)arr_val->value);
            if(log_ty_temp!=LT_UNKNOWN)
              (*bnf).log_types|=log_ty_temp; 

            sle_array=sle_array->_next;
          }
          break;
        case KEY_ACTION_OPTS:
          if(cpair->value->value_type!=JS_ARRAY)
            return -1;
          break;
        case KEY_CONFLICTS:
          if(cpair->value->value_type!=JS_STRING)
            return -1;
          break;
        case KEY_TOKENS:
          if(cpair->value->value_type!=JS_OBJECT)
            return -1;
          break;
        case KEY_START_SYM:
          if(cpair->value->value_type!=JS_STRING)
            return -1;
          (*bnf).start_symbol=(char*)cpair->value->value;
          break;
        /*************************************************
         * process pre and post includes.                *
         *************************************************/
        case KEY_PRE_INCLUDES:
          if(cpair->value->value_type!=JS_STRING)
            return -1;
          (*bnf).pre_includes=(char*)cpair->value->value;
          break;
        case KEY_POST_INCLUDES:
          if(cpair->value->value_type!=JS_STRING)
            return -1;
          (*bnf).post_includes=(char*)cpair->value->value;
          break;
        /*******************************************************
         * now undertake the job of parsing the bnf definition *
         *******************************************************/
        case KEY_GRAMMAR_DEF:
          if(cpair->value->value_type!=JS_STRING)
            return -1;
          if(jy_parse_grammar_def(bnf,(char*)cpair->value->value)==-1)
            return -1;
          break;
        case KEY_UNKNOWN:
        default:
          return -1;
      }
    }
    else
      return -1;
    sle=sle->_next;
  }
  return 1;
}

int jy_parse_grammar_def(bnf_grammar* bnf,
                         char* def_buffer)
{
   if(!bnf||!def_buffer)
     return -1;

   parse_items=parse_bnf(def_buffer);
   if(!parse_items||parse_items->_size<0)
     return -1;
  
  /***********************************************************
   * augment a start symbol. (this should really be replaced *
   * by the symbol specified in .jy)                         *
   ***********************************************************/
  bnf_parse_item* aug_start=new_bnf_parse_item("<$SS");
  slist_append(aug_start->products,(void*)new_slist());
  slist_append((slist*)aug_start->products->_head->_data,(void*)((bnf_parse_item*)parse_items->_head->_data)->head);
  slist_push(products->members,(void*)"<$SS");
  slist_push(parse_items,(void*)aug_start);
  if(convert_to_bnfg(bnf,parse_items,products,tokens,actions)==-1)
    return -1;
  return 1;
}