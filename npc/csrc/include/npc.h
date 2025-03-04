#ifndef _NPC_H_
#define _NPC_H_
#include <stdint.h>
#include <inttypes.h>
#include <stdbool.h>
#include <string.h>
#include<stdint.h>
#include<debug.h>
#include"utils.h"
// #define CONFIG_MBASE 0x80000000
// typedef uint32_t paddr_t;
void init_mem();
void init_isa();
uint8_t* guest_to_host(paddr_t paddr);
static inline paddr_t host_read(void *addr, int len) {
    switch (len) {
      case 1: return *(uint8_t  *)addr;
      case 2: return *(uint16_t *)addr;
      case 4: return *(uint32_t *)addr;
      default: ;
    }
  };
  #endif // _NPC_H_