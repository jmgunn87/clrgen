#ifndef PARSER_H
#define PARSER_H

#define SEMI_COLON 6
#define PRODUCT 7
#define COLON 8
#define ACTION 9
#define PIPE 10
#define TOKEN 11

#pragma once
#include "bnf_constructs.h"

#ifndef LPTOKENDECL_H
#define LPTOKENDECL_H

typedef struct lp_token
{
  int lex_id;
  char* lex_val;
}lp_token;

lp_token* new_token(int lid,char* lval);

#endif

slist* parse_bnf(char*);

#endif