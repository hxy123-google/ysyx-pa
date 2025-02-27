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
#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include<assert.h>
#include"npc.h"
static uint8_t *pmem = NULL;
uint8_t* guest_to_host(uint32_t paddr) { return pmem + paddr - CONFIG_MBASE; }

static uint32_t pmem_read(uint32_t addr, int len) {
  uint32_t ret = host_read(guest_to_host(addr), len);
  return ret;
}

// static void pmem_write(paddr_t addr, int len, word_t data) {
//   host_write(guest_to_host(addr), len, data);
// }

// static void out_of_bound(paddr_t addr) {
//   panic("address = " FMT_PADDR " is out of bound of pmem [" FMT_PADDR ", " FMT_PADDR "] at pc = " FMT_WORD,
//       addr, PMEM_LEFT, PMEM_RIGHT, cpu.pc);
// }

void init_mem() {
// #if   defined(CONFIG_PMEM_MALLOC)
  pmem = (uint8_t*)malloc(0x80000);
  assert(pmem);
// #endif
//   IFDEF(CONFIG_MEM_RANDOM, memset(pmem, rand(), CONFIG_MSIZE));
  //printf("physical memory area [" FMT_PADDR ", " FMT_PADDR "]", PMEM_LEFT, PMEM_RIGHT);
}


extern "C" uint32_t paddr_read(uint32_t addr, int len) {
   //printf("pread at " FMT_PADDR " len=%d\n", addr, len);
  //if (likely(in_pmem(addr))) 
  //printf("0x%x\n",addr);
  //if(addr==0) addr=0x80000000;
  //uint32_t t=pmem_read(addr,len);
  //printf("0x%x\n",t);
  return pmem_read(addr, len);
//   IFDEF(CONFIG_DEVICE, return mmio_read(addr, len));
//   out_of_bound(addr);
  //return 0;
}

// void paddr_write(paddr_t addr, int len, word_t data) {
//   //printf("pwrite at " FMT_PADDR " len=%d, data=" FMT_WORD "\n", addr, len, data);
//   if (likely(in_pmem(addr))) { pmem_write(addr, len, data); return; }
//   IFDEF(CONFIG_DEVICE, mmio_write(addr, len, data); return);
//   out_of_bound(addr);
// }
