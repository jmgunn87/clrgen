//beginning of stack data structure used by this parser
struct pstack_elem{void *_data;pstack_elem *_next;};
struct pcstack{size_t size;pstack_elem* first;};
pcstack* new_pcstack()
{
  pcstack* pcs((pcstack*)malloc(sizeof(pcstack)));
  memset((void*)pcs,0,sizeof(pcstack));
  pcs->size=0;
  pcs->first=0;
  return pcs;
}
void* peek_stack(pcstack* stack)
{
  return !stack||!stack->first?0:stack->first->_data;
}
void push_stack(pcstack *stack,void *data)
{
  if(!stack)return;
  pstack_elem* new_elem=(pstack_elem*)malloc(sizeof(pstack_elem));
  memset((void*)new_elem,0,sizeof(pstack_elem));
  new_elem->_data=data;
  if(!stack->first)
    new_elem->_next=0;
  else
    new_elem->_next=stack->first;
  (*stack).first=new_elem;
  (*stack).size+=1;
}
void* peek_stackn(pcstack* stack,
                  int index)
{
  int x=0;
  pstack_elem* curr_elem=stack->first;
  for(;x<=index;++x)
  {
    if(!curr_elem)return 0;
    if(x==index&&curr_elem)
      return curr_elem->_data;
    curr_elem=curr_elem->_next;
  }
  return 0;
}
void* pop_stack(pcstack *stack)
{
  if(!stack||!stack->first)return 0;
  void *ret=stack->first->_data;
  pstack_elem* temp=0;
  if(stack->first->_next)
  {
    temp=stack->first->_next;
    free((*stack).first);
    (*stack).first=temp;
    (*stack).size-=1;
  }
  else
  {
   free((*stack).first);
   (*stack).first=0;
   (*stack).size=0;
  }
  return ret;
}
void destroy_stack(pcstack* stack)
{
  pstack_elem *se_curr=stack->first,
              *se_next=0;
  while(se_curr&&se_curr->_next)
  {
    se_next=se_curr->_next;
    se_curr->_next=0;
    se_curr->_data=0;
    free(se_curr);
    se_curr=se_next;
  }
  (*stack).first=0;
  (*stack).size=-1;
}
  
//parser table declaration
int parse_table[1][7]=
{{0,0,0,0,0,0,2}};

//reduction actions switch function
int __prh(int* ri, pcstack* stack)
{
  switch(-*ri)
  {
    case 2:*ri=1;return -1;
    case 3:*ri=3;return -1;
    default:
      return-1;
  }
}

//the parser function itself
lp_token* new_token(int lid, char* lval)
{
  lp_token* ret=(lp_token*)malloc(sizeof(lp_token));
  ret->lex_id=lid;
  if(lval)
  {
    int len=strlen(lval);
    ret->lex_val=(char*)malloc(len+1);
    ret->lex_val[len]='\0';
    strcpy(ret->lex_val,lval);
  }else ret->lex_val=0;
  return ret;
}

void __parser(lp_token** input,
              int count)
{
  int top_state=0;
  int input_index=0;
  int accept_state=2;
  int pop_count=0;
  int action=0;

  //initialize the stack at state 0
  pcstack* parse_stack=new_pcstack();
  push_stack(parse_stack,(void*)new_token(0,0));

  while(true)
  {
    top_state=((lp_token*)peek_stack(parse_stack))->lex_id;
    if(input_index==count)action=parse_table[top_state][6];
    else if(input[input_index]->lex_id>=6)return;
    else action=parse_table[top_state][input[input_index]->lex_id];

    if(action==accept_state)//accept
    {
      action=-((lp_token*)peek_stack(parse_stack))->lex_id;
      __prh(&action,parse_stack);
      printf("accept\n");
      return;
    }
    if(action>0)//shift
    {
      //printf("shift\n");
      push_stack(parse_stack,(void*)new_token(action,input[input_index]->lex_val));
      ++input_index;
    }
    else if(action<0)//reduce
    {
      pop_count=__prh(&action,parse_stack);
      if(pop_count==-1)break;//catch errors here
      while(pop_count>0)
      {
        pop_stack(parse_stack);
        --pop_count;
      }
      push_stack(parse_stack,(void*)new_token(parse_table[((lp_token*)peek_stack(parse_stack))->lex_id][action],0));
      //printf("reduce\n");
    }
    else//error
    {
      printf("error\n");
      return;
    }
  }
}