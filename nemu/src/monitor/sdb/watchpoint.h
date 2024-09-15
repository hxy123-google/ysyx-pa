#ifndef __WATCHPOINT_H__  // 如果宏 __WATCHPOINT_H__ 没有定义
#define __WATCHPOINT_H__  // 定义宏 __WATCHPOINT_H__

// 头文件的内容放在这里，比如结构体和函数声明
typedef struct watchpoint {
  int NO;
  struct watchpoint *next;
  int old_value;
  int new_value;
  char expr[100];
} WP;

void init_wp_pool(void);
WP* new_wp(void);
void free_wp(int num);
void create_watchpoint(char * args);
void display_watchpoint();
#endif // 结束条件编译
