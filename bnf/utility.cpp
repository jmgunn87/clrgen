#pragma once
#include "stdafx.h"
#include "utility.h"

char* file_load(char* filename)
{
  FILE* sk_file=0;
  char* sk_buffer=0;
  long sk_size=0;

  /*************************
   * try and open the file *
   *************************/
  sk_file=fopen(filename,"r");
  if(!sk_file)
    return 0;

  /******************************************************
   * obtain the file size and allocate for its contents *
   ******************************************************/
  fseek(sk_file,0,SEEK_END);
  sk_size=ftell(sk_file);
  rewind(sk_file);
  sk_buffer=(char*)malloc(sizeof(char)*sk_size);
  
  /*****************************
   * read in the skeleton file *
   *****************************/
  sk_size=fread(sk_buffer,1,sk_size,sk_file);
  if(!sk_size)
    return 0;

  /******************************************
   * make sure we null terminate the buffer *
   * and close the file                     *
   ******************************************/
  sk_buffer[sk_size]='\0';
  fclose(sk_file);
  return sk_buffer;
}
