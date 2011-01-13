// bnf.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include "jsk.h"
#include "jy.h"

int main(int argc,
         char *argv[])
{
  if(argc<2)
    return -1;

  /**************************************
   * open and process a .jy file into a *
   * bnf_grammar structure.             *
   **************************************/
  bnf_grammar bnf;
  slist* jy_root=json_decode("jy_file.txt");
  if(json_to_bnf(jy_root,&bnf)==-1)
    return -1;
  
  /*****************************
   * construct LR(1) item sets *
   *****************************/
  freopen("out.txt","w+",stdout);
  cset* citems=bnf_construct_items(&bnf);
  freopen("CON","w",stdout);
  
  /**********************************************
   * produce our main product, the parser table *
   **********************************************/
  int** table=bnf_construct_parse_table(&bnf,citems);

  /**************************************************************
   * open a jsk, decode it and generate code using its skeleton *
   **************************************************************/
  skeleton_format skfmt;
  skfmt.sk_templates=new_slist();
  slist* jsk_root=json_decode(bnf.skeleton_fpath);
  if(json_to_skeleton(jsk_root,&skfmt)==-1)
    return -1;

  /***********************************************
   * run through all the specified templates and *
   * generate thier code                         *
   ***********************************************/
  skeleton_template* sktmp=0;
  slist_elem* sle=skfmt.sk_templates->_head;
  while(sle)
  {
    sktmp=(skeleton_template*)sle->_data;
    skeleton_generate(&bnf,
                      &skfmt,
                      sktmp->filename,
                      sktmp->template_ptr,
                      table,
                      citems->members->_size,
                      bnf.start_of_actions+1,
                      citems->members->_size+1);
    sle=sle->_next;
  }

	return 1;
}

