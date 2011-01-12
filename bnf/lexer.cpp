#pragma once
#include "stdafx.h"
#include "lexer.h"

slist* lex_bnf(char* str)
{
  int i=0;
  int sublen=0;
  char* substr=0;
  slist* lexed_inputs=new_slist();
  for(;i<strlen(str);++i)
    switch(str[i])
    {
      case ' ':case '\t':case '\n':
        break;
      case '<':
        for(sublen=0;i<strlen(str)&&str[++i]!='>';++sublen)
          if(str[i]=='<')printf("bnf parse error: nested product token.\n");
        substr=(char*)malloc(sublen+1);
        substr[sublen]='\0';
        strncpy(substr,&str[i-sublen],sublen);
        //printf("%s\n",substr);
        slist_append(lexed_inputs,(void*)new_token(PRODUCT,substr));
        break;
      case '(':
        for(sublen=0;i<strlen(str)&&str[++i]!=')';++sublen)
          if(str[i]=='(')printf("bnf parse error: nested token.\n");
        substr=(char*)malloc(sublen+1);
        substr[sublen]='\0';
        strncpy(substr,&str[i-sublen],sublen);
        //printf("%s\n",substr);
        slist_append(lexed_inputs,(void*)new_token(TOKEN,substr));
        break;
      case '{':
        for(sublen=0;i<strlen(str)&&str[++i]!='}';++sublen)
          if(str[i]=='{')printf("bnf parse error: nested action. \n");
        substr=(char*)malloc(sublen+1);
        substr[sublen]='\0';
        strncpy(substr,&str[i-sublen],sublen);
        //printf("%s\n",substr);
        slist_append(lexed_inputs,(void*)new_token(ACTION,substr));
        break;
      case ':':
        slist_append(lexed_inputs,(void*)new_token(COLON,":"));
        break;
      case ';':
        slist_append(lexed_inputs,(void*)new_token(SEMI_COLON,";"));
        break;
      case '|':
        slist_append(lexed_inputs,(void*)new_token(PIPE,"|"));
        break;
      default:
        break;
    }
  return lexed_inputs;
}