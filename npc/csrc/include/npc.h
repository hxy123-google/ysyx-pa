#ifndef _NPC_H_
#define _NPC_H_
#include <stdint.h>
#include <inttypes.h>
#include <stdbool.h>
#include <string.h>
#include<stdint.h>
#define CONFIG_MBASE 0x80000000
typedef uint32_t paddr_t;
extern void init_mem();
extern void init_isa();
extern uint32_t paddr_read(paddr_t addr, int len);
extern uint8_t* guest_to_host(paddr_t paddr);
static inline paddr_t host_read(void *addr, int len) {
    switch (len) {
      case 1: return *(uint8_t  *)addr;
      case 2: return *(uint16_t *)addr;
      case 4: return *(uint32_t *)addr;
      default: ;
    }
  };
  #endif // _NPC_H_