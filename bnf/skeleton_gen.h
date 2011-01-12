#ifndef SKELETON_GEN_H
#define SKELETON_GEN_H

#pragma once
#include "bnf_constructs.h"
#include "utility.h"

typedef struct _skeleton_template
{
  char* filename;
  char* template_ptr;

} skeleton_template;

typedef struct _skeleton_format
{
  char* comment_start;
  char* comment_end;

  char* token_synth_val;
  char* stack_access_start;
  char* stack_access_end;

  char* tbl_start;
  char* tbl_row_start;
  char* tbl_entry_delim;
  char* tbl_row_end;
  char* tbl_end;

  char* reduce_act_case_start;
  char* reduce_act_case_end;
  char* reduce_act_ret_start;
  char* reduce_act_ret_end;
  char* reduce_act_return_start;
  char* reduce_act_return_end;

  char* action_start;
  char* action_end;
  
  slist* sk_templates;/* of skeleton_templates */;

} skeleton_format;

int skeleton_generate(bnf_grammar* bnf,
                      skeleton_format* skfmt,
                      char* filename,
                      char* template_ptr,
                      int** ptbl,
                      int ptbl_row_dim,
                      int ptbl_col_dim,
                      int accept_state);
 
int write_parse_table(FILE* skf,
                      int** ptbl,
                      int ptbl_row_dim,
                      int ptbl_col_dim,
                      skeleton_format* skfmt);
int write_action(FILE* f,
                 char* action,
                 int prod_len,
                 skeleton_format* skfmt);

int write_reduction_handler(FILE* skf,
                            bnf_grammar* bnf,
                            skeleton_format* skfmt);


#endif