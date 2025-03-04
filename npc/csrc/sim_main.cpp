#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include "Vysyx_25020052_top.h"
#include "verilated_vcd_c.h"
#include "svdpi.h"
#include <debug.h>
#include "npc.h"
#include "verilated_dpi.h"
// extern NPCState npc_state = { .state = NPC_STOP };
static Vysyx_25020052_top dut;
static uint32_t *pmem = NULL;
VerilatedVcdC *tfp; // 导出vcd波形需要加此语句
VerilatedContext *contextp;
int is_exit_status_bad();
void init_monitor(int argc, char *argv[]);
uint32_t* reg_c=nullptr;
extern "C" void get_reg(const svOpenArrayHandle r){
  reg_c = (uint32_t *)(((VerilatedDpiOpenVar*)r)->datap());
}
// enum { NPC_RUNNING, NPC_STOP, NPC_END, NPC_ABORT, NPC_QUIT };
static void single_cycle()
{
  dut.clk = 0;
  dut.pc=2147483648;
  dut.eval();
  dut.clk = 1;
  dut.eval();
 }
extern "C" void npc_trap()
{
  npc_state.state = NPC_END;
  npc_state.halt_pc = dut.pc;
  npc_state.halt_ret = 0;
  printf("%x\n", npc_state.halt_pc);
}
extern "C" void set_npc_state(int state, paddr_t pc, int halt_ret) {
  //difftest_skip_ref();
  npc_state.state = state;
  npc_state.halt_pc = pc;
  npc_state.halt_ret = halt_ret;
}
static void reset(int n)
{
  dut.rst = 1;
  while (n-- > 0)
    single_cycle();
  dut.rst = 0;
}
void init_verilator()
{
  Verilated::traceEverOn(true); // 导出vcd波形需要加此语句
  tfp = new VerilatedVcdC();    // 导出vcd波形需要加此语句
  contextp = new VerilatedContext;
  dut.trace(tfp, 10);
  tfp->open("wave.vcd"); // 打开vcd
  reset(10);
};
void exec_once(){
  dut.clk=0;
  dut.eval();
  tfp->dump(contextp->time());
  contextp->timeInc(1);

  dut.clk=1;
  dut.eval();
  tfp->dump(contextp->time());
  contextp->timeInc(1);
}
void cpu_exec();
// { 
//   npc_state.state=NPC_RUNNING;
//   for (; npc_state.state == NPC_RUNNING;)
//   {
//     exec_once();
//   }
//   switch (npc_state.state)
//   {
//   case NPC_RUNNING:
//     npc_state.state = NPC_STOP;
//     break;

//   case NPC_END:
//   case NPC_ABORT:
//     Log("npc: %s at pc = " FMT_WORD,
//         (npc_state.state == NPC_ABORT ? ANSI_FMT("ABORT", ANSI_FG_RED) : (npc_state.halt_ret == 0 ? ANSI_FMT("HIT GOOD TRAP", ANSI_FG_GREEN) : ANSI_FMT("HIT BAD TRAP", ANSI_FG_RED))),
//         npc_state.halt_pc);
//     // fall through
//   // case NPC_QUIT:
//   //   statistic();
//   }
// };
int is_exit_status_bad(); 
// {
//   int good = (npc_state.state == NPC_END && npc_state.halt_ret == 0) ||
//     (npc_state.state == NPC_QUIT);
//   return !good;
// }
int main(int argc, char *argv[])
{
  for (int i = 0; i < argc; i++)
  {
    Log("%s\n", argv[i]);
  }
  init_monitor(argc, argv);
  init_verilator();
  cpu_exec();
  tfp->close();
  for(int i=0;i<32;i++){
    printf("%d %d\n",i,reg_c[i]);
  }
  return is_exit_status_bad();
}