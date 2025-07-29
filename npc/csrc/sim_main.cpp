#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include "Vysyx_25020052_top.h"
#include "verilated_vcd_c.h"
#include "svdpi.h"
#include <debug.h>
#include "npc.h"
// #include "Vysyx_25020052_top__Dpi.h"
 #include "verilated_dpi.h"
// extern NPCState npc_state = { .state = NPC_STOP };
static Vysyx_25020052_top dut;
static uint32_t *pmem = NULL;
VerilatedVcdC *tfp; // 导出vcd波形需要加此语句
VerilatedContext *contextp;
int is_exit_status_bad();
void isa_reg_display();
void init_monitor(int argc, char *argv[]);
extern uint32_t* reg_c;
extern "C" void get_reg(const svOpenArrayHandle r){
  reg_c = (uint32_t *)(((VerilatedDpiOpenVar*)r)->datap());
}

// enum { NPC_RUNNING, NPC_STOP, NPC_END, NPC_ABORT, NPC_QUIT };
// extern void get_reg();

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
extern "C" void set_npc_state(int state, int pc, int halt_ret) {
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
  printf("pc:%x ",dut.pc);
  //isa_reg_display() ;
  printf("%x\n",dut.inst);
}
void sdb_mainloop();
//void cpu_exec();
int main(int argc, char *argv[])
{
  for (int i = 0; i < argc; i++)
  {
    Log("%s\n", argv[i]);
  }
  init_monitor(argc, argv);
  init_verilator();
  sdb_mainloop();
  //cpu_exec();
  tfp->close();
  // for(int i=0;i<32;i++){
  //   printf("%d %d\n",i,reg_c[i]);
  // }
  return is_exit_status_bad();
}