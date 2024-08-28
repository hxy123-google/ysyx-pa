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

#include <isa.h>

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <regex.h>
#include <memory/vaddr.h>
#include <memory/paddr.h>
enum {
  TK_NOTYPE = 256, TK_EQ=0,TK_NEQ=1,TK_REG=3,TK_HEX=4,TK_NUM=5,TK_plus=6,
  TK_minus=7,TK_mul=8,TK_del=9,TK_lef=10,TK_rig=11,TK_DEFER=12,TK_NEG=13
  
  /* TODO: Add more token types */

};

static struct rule {
  const char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */
  
  {" +", TK_NOTYPE},    
  {"\\+", TK_plus},        
  {"==", TK_EQ},        
  {"-",TK_minus},           
  {"\\*",TK_mul} ,          
  {"/",TK_del} ,            
  {"\\(",TK_lef},
  {"\\)",TK_rig},
  {"!=",TK_NEQ},
  {"\\$(a[0-7]|\\$0|ra|[sgt]p|t[0-6]|a[0-7]|s([0-9]|1[0-1]))",TK_REG},
  {"0[xX][0-9a-fA-F]+",TK_HEX},
  {"[0-9]+",TK_NUM},
};

#define NR_REGEX ARRLEN(rules)

static regex_t re[NR_REGEX] = {};

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[32];
} Token;

static Token tokens[32] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;
static bool check_parenthess(int p,int q,bool *ok){
  if(tokens[p].type!=TK_lef||tokens[q].type!=TK_rig){
    return false;
  }
  int left_num=0;
  for(int i=p;i<=q;i++){
    //printf("%d %d\n",i,left_num);
    if(tokens[i].type==TK_lef) left_num++;
    if(tokens[i].type==TK_rig){
      left_num--;
      if(left_num==0&&i!=q) return false;
      //if(left_num<0) return false;
    }
  }
  if(left_num!=0){
    *ok=false;
    return false;
  }
  return true;

};
static word_t eval(int p, int q,bool *ok) { 
  *ok=true;
  if (p > q) {
    *ok=false;
    return 0;
    /* Bad expression */
  }
  else if (p == q) {
    word_t num;
    if(tokens[p].type==TK_NUM){
      sscanf(tokens[p].str,"%d",&num);
      return num;
    }
    if(tokens[p].type==TK_HEX){
      num=strtol(tokens[p].str,NULL,16);
      //printf("%d",num);
     return strtol(tokens[p].str, NULL, 16);
    }
    if(tokens[p].type==TK_REG){
      bool t=true;
      num=isa_reg_str2val(tokens[p].str,&t);
      return num;
    }
    if(tokens[p].type!=TK_REG&&tokens[p].type!=TK_HEX&&tokens[p].type!=TK_NUM){
      *ok=false;
      return 0;
    }
    /* Single token.
     * For now this token should be a number.
     * Return the value of the number.
     */
  }
  else if (check_parenthess(p, q,ok) == true) {
    /* The expression is surrounded by a matched pair of parentheses.
     * If that is the case, just throw away the parentheses.
     */
    return eval(p + 1, q - 1,ok);
  }
  else {
    int is_first=-1;//-1代表没有，0代表*和-，1代表* /，2代表+ -
    int op=-1;
    int left_num=0;
    for(int i=p;i<=q;i++){
      if(tokens[i].type==TK_DEFER||tokens[i].type==TK_NEG){
        if(left_num==0&&is_first==-1){
          op=i;
          is_first=0;
        }
        else continue;
      }
      if(tokens[i].type==TK_NOTYPE){
        continue;
      }
      if(tokens[i].type==TK_lef){
        left_num++;
      }
      if(tokens[i].type==TK_rig){
        left_num--;
      }
      if(tokens[i].type==TK_NUM||tokens[i].type==TK_HEX||tokens[i].type==TK_REG){
        continue;
      }
      if(tokens[i].type==TK_plus||tokens[i].type==TK_minus){
        if(left_num!=0) continue;
        else {
          op=i;
          is_first=2;
        }
      }
      if(tokens[i].type==TK_mul||tokens[i].type==TK_del){
        if(left_num==0){
          if(is_first==-1||is_first==0||is_first==1){
            op=i;
            is_first=1;
          }
          else continue;
        } 
        else continue;
      }
    }
    bool ok1,ok2;
    int val1,val2;
    val2 = eval(op + 1, q,&ok2);
    if(ok2==false){
      *ok=false;
      return 0;
    }
    if(tokens[op].type==TK_DEFER||tokens[op].type==TK_NEG){
      if(tokens[op].type==TK_NEG){
        return -1*val2;
      }
      else{ return vaddr_read(val2,4);}
    }
    val1 = eval(p, op - 1,&ok1);
    if(ok1==false){
      *ok=false;
      return 0;
    }
    switch (tokens[op].type) {
      case TK_plus: 
        printf("符号为%d %d %d %d\n",tokens[op].type,val1,val2,val1+val2);
        return val1 + val2;
      case TK_minus: 
        printf("符号为%d %d %d %d\n",tokens[op].type,val1,val2,val1-val2);
        return val1-val2;
      case TK_mul: 
       printf("符号为%d %d %d %d\n",tokens[op].type,val1,val2,val1*val2);
       return val1*val2;
      case TK_del: 
       printf("符号为%d %d %d %d\n",tokens[op].type,val1,val2,val1/val2);
       if(val2==0){
        *ok=false;
        return 0;
       }
       return val1/val2;
      default: assert(0);
    }
  }
  return 0;
}
static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;
  nr_token = 0;
  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;
        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);
        position += substr_len;
        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */
        switch (rules[i].token_type) {
          case TK_EQ:
            tokens[nr_token++].type=rules[i].token_type;
            break;
          case TK_NEQ:
            tokens[nr_token++].type=rules[i].token_type;
            break;
          case TK_REG:
            tokens[nr_token].type=rules[i].token_type;
            strncpy(tokens[nr_token].str, substr_start, substr_len);
            nr_token++;
            break;
          case TK_HEX:
            tokens[nr_token].type=rules[i].token_type;
            strncpy(tokens[nr_token].str, substr_start, substr_len);
            tokens[nr_token].str[substr_len]='\0';
            nr_token++;
            break;
          case TK_NUM:
            tokens[nr_token].type=rules[i].token_type;
            strncpy(tokens[nr_token].str, substr_start, substr_len);
            tokens[nr_token].str[substr_len]='\0';
            nr_token++;
            break;
          case TK_plus:
            tokens[nr_token++].type=rules[i].token_type;
            break;
          case TK_minus:
          if(nr_token==0||(tokens[nr_token-1].type!=TK_HEX&&tokens[nr_token-1].type!=TK_NUM
            &&tokens[nr_token-1].type!=TK_REG&&tokens[nr_token-1].type!=TK_rig))
            tokens[nr_token++].type=TK_NEG;
            else tokens[nr_token++].type=rules[i].token_type;
            break;
          case TK_mul:
            if(nr_token==0||(tokens[nr_token-1].type!=TK_HEX&&tokens[nr_token-1].type!=TK_NUM
            &&tokens[nr_token-1].type!=TK_REG&&tokens[nr_token-1].type!=TK_rig))
            tokens[nr_token++].type=TK_DEFER;
            else tokens[nr_token++].type=rules[i].token_type;
            break;
          case TK_del:
            tokens[nr_token++].type=rules[i].token_type;
            break;
          case TK_lef:
            tokens[nr_token++].type=rules[i].token_type;
            break;
          case TK_rig:
            tokens[nr_token++].type=rules[i].token_type;
            break;
          case TK_NOTYPE:
            break;
          default:
            printf("np matches");
            break;
          //default: TODO();
        }
        break;
      }
    }
    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }
  return true;
}


word_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }
  // for(int i=0;i<nr_token;i++){
  //   printf("类型：%d 值为 %s\n",tokens[i].type,tokens[i].str);
  // }
  word_t ans=eval(0,nr_token-1,success);
  assert(*success);
  printf("expr:%u\n",ans);
  
  /* TODO: Insert codes to evaluate the expression. */
  //TODO();

  return ans;
}
