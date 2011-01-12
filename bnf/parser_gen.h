#ifndef PARSER_GEN_H
#define PARSER_GEN_H

#pragma once
#include "stdafx.h"
#include "bnf_constructs.h"

void write_stack_source(FILE* f,
                        unsigned long initial_size,
                        unsigned long growth_amount);
void write_header(FILE* f,
                  bnf_grammar* bnf,
                  char* parser_fn_name);
void write_parser(bnf_grammar* bnf,
                  int** parse_table,
                  int rows,
                  int cols);
void write_parse_table(FILE* f,
                       int** table,
                       int rows,
                       int cols,
                       char* table_name);
void write_action(FILE* f,
                  char* action,
                  int prod_len);
void write_reduction_handler(FILE* f,
                             bnf_grammar* bnf);
void write_parser_fn(FILE* f,
                     bnf_grammar* bnf,
                     char* function_name,
                     char* table_name,
                     int accept_symbol);

#endif