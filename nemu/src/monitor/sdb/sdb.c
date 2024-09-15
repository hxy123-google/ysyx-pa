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
#include <cpu/cpu.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "sdb.h"
#include <memory/vaddr.h>
#include <memory/paddr.h>
#include "watchpoint.h"
 static int is_batch_mode = false;
void init_regex();
/* We use the `readline' library to provide more flexibility to read from stdin. */
static char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}


static int cmd_q(char *args) {
  nemu_state.state=NEMU_QUIT;
  return -1;
}

static int cmd_help(char *args);
static int cmd_si(char *args){
  int step;
  if(args==NULL) step=1;
  else sscanf(args,"%d",&step);
  cpu_exec(step);
  return 0;
}
static int cmd_info(char *args){
  char SUBCMD;
  sscanf(args,"%c",&SUBCMD);
  if(SUBCMD=='r') isa_reg_display();
  else if(SUBCMD=='w'){
    display_watchpoint();
  }
  return 0;
}
// static int cmd_x(char *args) {
//   char* s_num1 = strtok(NULL, " ");
//   if (s_num1 == NULL) {
//     return 0;
//   }
//   int num1 = atoi(s_num1);
//   char* s_num2 = strtok(NULL, " ");
//   if (s_num2 == NULL) {
//     return 0;
//   }
//   if (strlen(s_num2) <= 2) {
//     //DAssert(0, "x的第二个参数必须以0x开头,怎么会长度<=2呢?");
//   }
//   // 因为开头有一个0x, 我们需要去掉它, 不然解析会出错
//   // BUG here: atoi默认是十进制, 但是这里应该是十六进制
//   // paddr_t addr = atoi(s_num2+2);
//   paddr_t addr = (paddr_t)strtol(s_num2+2, NULL, 16);
//   // 开始扫描
//   printf("%s\t\t%-34s%-32s\n", "addr", "16进制", "10进制");
//   printf("%s:\t", s_num2);
//   for (int i = 1; i <= num1<<2; i++) {
//     // 因为这个是一个字节数组, 所以我们需要四个为一组进行扫描
//     if (i%4 != 0) {
//       printf("0x%-4x ", paddr_read(addr + i - 1, 1));
//     } else {
//       printf("0x%-4x\t", paddr_read(addr + i - 1, 1));
//       for (int j = i - 3; j <= i; j++) {
//         // 打印十进制的
//         printf("%-4d ", paddr_read(addr + j - 1, 1));
//       }
//       printf("\n");
//       if (i == num1<<2) {
//         printf("\n");
//       } else {
//         printf("0x%x:\t", addr + i);
//       }
//     }
//   }return 0;
// }
static int cmd_x(char * args){
  char * n=strtok(args," ");
  char * base_addr=strtok(NULL," ");
  int len;
  sscanf(n,"%d",&len);
  paddr_t addr;
  sscanf(base_addr,"%x",&addr);
  //printf("%d %x",len,addr);
  for(int i=0;i<len;i++){
    printf("地址：0x%x ",addr);
    for(int j=0;j<4;j++){
      printf("0x%x ",paddr_read(addr+j,1));
    }
    addr+=4;
    printf("\n");

  }
  return 0;
}
static int cmd_p(char*args){
    if(args == NULL){
         printf("No args\n");
         return 0;
     }
    bool flag = false;
    expr(args, &flag);

    return 0;
}
static int cmd_w(char*args){
  create_watchpoint(args);
  return 0;
}
static int cmd_d(char*args){
  free_wp(atoi(args));
  return 0;
}
static struct {
  const char *name;
  const char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display information about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },
  {"si","让程序单步执行N条指令后暂停执行, 当N没有给出时, 缺省为1",cmd_si},
  {"info", "info r: 打印寄存器状态\ninfo w :打印监视点信息",cmd_info},
  {"x","求出表达式EXPR的值, 将结果作为起始内存地址, 以十六进制形式输出连续的N个4字节",cmd_x},
  {"p","求出表达式EXPR的值, EXPR支持的运算请见调试中的表达式求值小节",cmd_p},
  {"w","当表达式EXPR的值发生变化时, 暂停程序执行",cmd_w},
  {"d","删除序号为N的监视点",cmd_d}
  /* TODO: Add more commands */
  
};

#define NR_CMD ARRLEN(cmd_table)

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

void sdb_set_batch_mode() {
  is_batch_mode = true;
}

void sdb_mainloop() {
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }

  for (char *str; (str = rl_gets()) != NULL; ) {
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef CONFIG_DEVICE
    extern void sdl_clear_event_queue();
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}
void test_expr() {
  FILE *fp = fopen("/home/hxy/Desktop/ysyx-workbench/nemu/tools/gen-expr/build/input", "r");
  if (fp == NULL) perror("test_expr error");

  char *e = NULL;
  word_t correct_res;
  size_t len = 0;
  ssize_t read;
  bool success = false;

  while (true) {
    if(fscanf(fp, "%u ", &correct_res) == -1) break;
    e=NULL;
    read = getline(&e, &len, fp);
    e[read-1] = '\0';
    //printf("e为%c，read为%ld\n",e[read-2],read);
    word_t res = expr(e, &success);
    assert(success);
    if (res != correct_res) {
      puts(e);
      printf("expected: %u, got: %u\n", correct_res, res);
      assert(0);
    }
  }

  fclose(fp);
  if (e) free(e);

  Log("expr test pass");
}
void init_sdb() {
  /* Compile the regular expressions. */
  init_regex();
  //test_expr();
  /* Initialize the watchpoint pool. */
  init_wp_pool();
}
