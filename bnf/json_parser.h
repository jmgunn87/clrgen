#ifndef JSON_PARSER_H
#define JSON_PARSER_H

#pragma once
#include "slist.h"
#include <ctype.h>

#define JS_OBJECT 4656
#define JS_ARRAY 4657
#define JS_LBRACE 7
#define JS_RBRACE 8
#define JS_COMMA 9
#define JS_STRING 10
#define JS_COLON 11
#define JS_LBRACKET 12
#define JS_RBRACKET 13
#define JS_NUMBER 14
#define JS_TRUE 15
#define JS_FALSE 16
#define JS_NULL 17

typedef struct _json_value
{
  int value_type;
  void* value;

} json_value;

typedef struct _json_pair
{
  char* key;
  json_value* value;

} json_pair;

#ifndef LPTOKENDECL_H
#define LPTOKENDECL_H

typedef struct lp_token
{
  int lex_id;
  void* lex_val;
}lp_token;

lp_token* new_token(int lid,void* lval);

#endif

slist* json_decode(char* filepath);

#endif
