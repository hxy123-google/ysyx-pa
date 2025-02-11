#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include "Vtop.h"
#include "verilated_vcd_c.h"
#include "svdpi.h"
#include<iostream>
using namespace std;
static Vtop dut;
uint32_t* init_pmem(uint32_t size);
uint32_t guest_to_host(uint32_t addr);
uint32_t get_inst(uint32_t * memory,uint32_t paddr);
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
int main(int argc,char **argv)
{   //bool is_first=true;
    uint32_t *memory;
    memory=init_pmem(4);
    // Verilated::commandArgs(argc,argv);
    Verilated::traceEverOn(true); //导出vcd波形需要加此语句
    VerilatedVcdC* tfp = new VerilatedVcdC(); //导出vcd波形需要加此语句
    VerilatedContext* contextp = new VerilatedContext;
    dut.trace(tfp, 10);
    tfp->open("wave.vcd"); //打开vcd
    reset(10);
    
    for(;npc_state==running;){
        dut.clk=!dut.clk;
        dut.inst=get_inst(memory,dut.pc);
        dut.eval();
        tfp->dump(contextp->time());
        contextp->timeInc(1);
    }
     tfp->close();
     return 0;
}