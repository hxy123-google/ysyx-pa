/***************************************************************************************
* Copyright (c) 2014-2022 Zihao Yu, Nanjing University
*
* NEMU is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*          http://license.coscl.org.cn/MulanPSL2
*
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
*
* See the Mulan PSL v2 for more details.
***************************************************************************************/

#include "sdb.h"

#define NR_WP 32

typedef struct watchpoint {
  int NO;//表示监视点的序号
  struct watchpoint *next;
  int old_value;
  int new_value;
  char expr[100];
  /* TODO: Add more members if necessary */

} WP;

static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;
static WP *n_tail=NULL, *f_tail=NULL;
void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = (i == NR_WP - 1 ? NULL : &wp_pool[i + 1]);
    if(i==NR_WP-1) f_tail=&wp_pool[i];
  }

  head = NULL;
  free_ = wp_pool;
}
WP* new_wp(){
  WP *temp=NULL;
  if(free_==NULL){
    printf("没有空余的监视点\n");
    assert(0);
  }
  temp=free_;
  free_=free_->next;
  if(free_==NULL) f_tail=NULL;
  if(n_tail==NULL){
    temp->NO=1;
    head=temp;
    n_tail=temp;
  }else{
    n_tail->next=temp;
    temp->NO=n_tail->NO+1;
    n_tail=temp;
  }
  temp->next=NULL;
  return temp;
}
void free_wp(int num){
  WP* p=NULL;
  WP* temp=NULL;
  if(num==head->NO){
    temp=head;
    if(head->NO==n_tail->NO){
      head=NULL;
      n_tail=NULL;
    }else{
      head=head->next;
    }
    temp->next=NULL;
    if(free_==NULL){
      free_=temp;
      f_tail=temp;
    }else{
      f_tail->next=temp;
      f_tail=f_tail->next;
    }
    printf("删除成功\n");
    return;
  }
  for(p=head;p->next!=NULL;p=p->next){
    if(p->next->NO==num){
      if(p->next->next==NULL) n_tail=p;
      temp=p->next;
      p->next=p->next->next;
      temp->next=NULL;
      break;
    }
  }
  if(temp==NULL){
    printf("没有这个监视点\n");
    return;
  }
  if(free_==NULL){
    free_=temp;
    f_tail=temp;
  }else{
    f_tail->next=temp;
    f_tail=f_tail->next;
  }
  printf("删除成功\n");
  return;
}
void create_watchpoint(char * args){
  WP*p=new_wp();
  strcpy(p->expr,args);
  bool success=true;
 int tmp= expr(p->expr,&success);
  if(success) p -> old_value = tmp;
  else printf("创建watchpoint的时候expr求值出现问题\n");
  printf("Create watchpoint No.%d success.\n", p -> NO);
}
void display_watchpoint(){
  for(WP* p=head;p!=NULL;p=p->next){
      printf("Watchpoint.No: %d, expr = \"%s\", now_value = %d\n",
     p->NO, p->expr,p->old_value);
    }
}
void wp_check()
{
	WP* pnode = NULL;
	pnode = head;
	while(pnode != NULL)
	{
		bool success = true;
		word_t expr_result = expr(pnode -> expr, &success);
		Log("the previous_value of watchpoint %s is %u", pnode -> expr, pnode -> old_value);
		Log("the current value of watchpoint %s is %u", pnode -> expr, expr_result);
		if(expr_result != pnode -> old_value)
		{ 
			printf("The program has stopped due to watchpoint diff\n");
			nemu_state.state = NEMU_STOP;
			return ;
		}
    pnode=pnode->next;
	}
}

/* TODO: Implement the functionality of watchpoint */

