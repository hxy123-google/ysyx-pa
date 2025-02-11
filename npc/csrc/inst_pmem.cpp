#include <stdlib.h>
#include<string.h>
#include <stdint.h>
static const uint32_t img [] = {
  0b00000000010100000000000010010011,
  0b00000000000100000000000100010011,
  0b00000000001000000000000100010011,
  0b00000000010100001000000100010011,
  0b00000000000100000000000001110011
};
uint32_t *init_pmem(uint32_t size){
  uint32_t *memory=(uint32_t*)malloc(sizeof(uint32_t)*size);
  memcpy(memory,img,sizeof(img));
  return memory;
}
uint32_t guest_to_host(uint32_t addr){
  return addr-0x80000000;
}
uint32_t get_inst(uint32_t * memory,uint32_t paddr){
  uint32_t vaddr =guest_to_host(paddr);
  return memory[vaddr/4];
}
