#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include "Vtop.h"
#include "verilated_vcd_c.h"
#include "svdpi.h"
#include "npc.h"
static Vtop dut;
static uint32_t *pmem = NULL;
VerilatedVcdC* tfp; //导出vcd波形需要加此语句
VerilatedContext* contextp;


static void single_cycle() {
    dut.clk = 0;dut.eval();
    dut.clk = 1; dut.eval();
  }
  enum Npc_State {
        running,
        stop
};
static  Npc_State npc_state=running;
extern "C" void npc_trap(){
  npc_state=stop;
  printf("%x\n",dut.pc); 
}
  static void reset(int n) {
    dut.rst = 1;
    while (n -- > 0) single_cycle();
    dut.rst = 0;
  }
void init_verilator(){
  Verilated::traceEverOn(true); //导出vcd波形需要加此语句
    tfp = new VerilatedVcdC(); //导出vcd波形需要加此语句
    contextp = new VerilatedContext;
    dut.trace(tfp, 10);
    tfp->open("wave.vcd"); //打开vcd
    reset(10);
}
void cpu_exec(){
  for(;npc_state==running;){
    dut.clk=!dut.clk;
    dut.inst=paddr_read(dut.pc,4);
    dut.eval();
    tfp->dump(contextp->time());
    contextp->timeInc(1);
}
};
int main(int argc,char **argv){
    init_mem(); 
    init_isa();
    init_verilator();
    cpu_exec();
    tfp->close();
    return 0;
}