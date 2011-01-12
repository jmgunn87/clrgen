#ifndef JY_H
#define JY_H

#pragma once
#include "parser.h"
#include "json_parser.h"

typedef enum _TBLCMPR_TYPE
{
  TBLCMPR_NONE=1,
  TBLCMPR_PAIR_DUPLICATES=2,
  TBLCMPR_UNKNOWN=128

} TBLCMPR_TYPE;

typedef enum _LOGTYPE
{
  LT_NONE=0,
  LT_CONSOLE=1,
  LT_LOGFILE=2,
  LT_ALL=64,
  LT_UNKNOWN=128

} LOGTYPE;

typedef enum _JSON_BNF_KEY
{
  KEY_GRAMMAR_NAME=1,
  KEY_AUTHOR=2,
  KEY_COMPRESSION=3,
  KEY_SKPATH=4,
  KEY_OUT_DIR=5,
  KEY_LOG_NAME=6,
  KEY_LOG_TYPES=7,
  KEY_ACTION_OPTS=8,
  KEY_CONFLICTS=9,
  KEY_TOKENS=10,
  KEY_START_SYM=11,
  KEY_PRE_INCLUDES=12,
  KEY_POST_INCLUDES=13,
  KEY_GRAMMAR_DEF=14,
  KEY_UNKNOWN=1987

} JSON_BNF_KEY;

TBLCMPR_TYPE get_table_compression_type(char type);

LOGTYPE get_log_type(char type);

JSON_BNF_KEY get_key_type(char* key);

int jy_parse_grammar_def(bnf_grammar* bnf,
                         char* def_buffer);

int json_to_bnf(slist* root,
                bnf_grammar* bnf);



#endif