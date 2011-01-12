#pragma once
#include "stdafx.h"
#include "jsk.h"

SKFMT_TYPE get_format_key_type(char* key)
{
  if(!key)
    return SKFMT_UNKNOWN;

  if(strcmp(key,"comment_fmt")==0)
    return SKFMT_COMMENT_FMT;
  else if(strcmp(key,"stack_access_fmt")==0)
    return SKFMT_STACK_ACCESS_FMT;
  else if(strcmp(key,"table_fmt")==0)
    return SKFMT_TBL_FMT;
  else if(strcmp(key,"action_fmt")==0)
    return SKFMT_ACTION_FMT;
  else
    return SKFMT_UNKNOWN;
}

int json_to_sk_template(slist* root,
                        skeleton_template* sktmp)
{
  slist_elem* sle=0;
  json_pair* cpair=0;

  if(!root||!root->_size||!sktmp)
    return -1;

  sle=root->_head;
  while(sle)
  {
    cpair=(json_pair*)sle->_data;
    if(strcmp(cpair->key,"content")==0&&
       cpair->value->value_type==JS_STRING)
    {
      (*sktmp).template_ptr=(char*)cpair->value->value;
    }
    else if(strcmp(cpair->key,"filename")==0&&
            cpair->value->value_type==JS_STRING)
    {
      (*sktmp).filename=(char*)cpair->value->value;
    }
    else
      return -1;

    sle=sle->_next;
  }
  return 1;
}
int json_to_skeleton(slist* root,
                     skeleton_format* skfmt)
{
  slist_elem* sle=0;
  slist_elem* sle_array=0;
  skeleton_template* sktmp=0;
  json_value* arr_val=0;
  json_pair* cpair=0;
  int arr_pos=0;

  if(!root||!root->_size||!skfmt)
    return -1;

  sle=root->_head;
  while(sle)
  {
    if(sle->_data)
    {
      cpair=(json_pair*)sle->_data;
      /********************************************
       * if the value type is an array then try   *
       * and parse the correct formatting details *
       ********************************************/
      if(cpair->value->value_type==JS_ARRAY)
      {
        arr_pos=0;
        sle_array=((slist*)cpair->value->value)->_head;
        while(sle_array)
        {
          arr_val=(json_value*)sle_array->_data;
          if(arr_val->value_type!=JS_STRING)
            return -1;

          switch(get_format_key_type(cpair->key))
          {
            case SKFMT_COMMENT_FMT:
              if(((slist*)cpair->value->value)->_size>2)
                return -1;
              if(arr_pos==0)
                (*skfmt).comment_end=(char*)arr_val->value;
              else if(arr_pos==1)
                (*skfmt).comment_start=(char*)arr_val->value;
              break;
            case SKFMT_STACK_ACCESS_FMT:
              if(((slist*)cpair->value->value)->_size>3)
                return -1;
              if(arr_pos==0)
                (*skfmt).stack_access_end=(char*)arr_val->value;
              else if(arr_pos==1)
                (*skfmt).stack_access_start=(char*)arr_val->value;
              else if(arr_pos==2)
                (*skfmt).token_synth_val=(char*)arr_val->value;
              break;
            case SKFMT_TBL_FMT:
              if(((slist*)cpair->value->value)->_size>5)
                return -1;
              if(arr_pos==0)
                (*skfmt).tbl_end=(char*)arr_val->value;
              else if(arr_pos==1)
                (*skfmt).tbl_row_end=(char*)arr_val->value;
              else if(arr_pos==2)
                (*skfmt).tbl_entry_delim=(char*)arr_val->value;
              else if(arr_pos==3)
                (*skfmt).tbl_row_start=(char*)arr_val->value;
              else if(arr_pos==4)
                (*skfmt).tbl_start=(char*)arr_val->value;
              break;
            case SKFMT_ACTION_FMT:
              if(((slist*)cpair->value->value)->_size>2)
                return -1;
              if(arr_pos==0)
                (*skfmt).action_end=(char*)arr_val->value;
              else if(arr_pos==1)
                (*skfmt).action_start=(char*)arr_val->value;
              break;
            default:
              break;/*unknown key*/
          }   
          ++arr_pos;
          sle_array=sle_array->_next;
        }
      }
      /********************************************
       * if the value type is a string then this  *
       * will hopefully be the correct key for a  *
       * skeleton template.                       *
       ********************************************/
      else if(cpair->value->value_type==JS_OBJECT)
        if(strcmp(cpair->key,"skeleton_def")==0)
        {
          sktmp=(skeleton_template*)malloc(sizeof(skeleton_template));
          if(json_to_sk_template((slist*)cpair->value->value,sktmp)==-1)
            return -1;
          /************************************************************
           * store this template in the templates list for generation *
           ************************************************************/
          else
            slist_append((*skfmt).sk_templates,(void*)sktmp);
        }
    }
    else
      return -1;
    sle=sle->_next;
  }
  return 1;
}