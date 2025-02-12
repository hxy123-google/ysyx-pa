#include <stdlib.h>
#include<string.h>
#include <stdint.h>
#include "npc.h"
static const uint8_t img [] = {
  0b10010011,
  0b00000000,
  0b01010000,
  0b00000000,
  0b00010011,
  0b00000001,
  0b00010000,
  0b00000000,
  0b01110011,
  0b00000000,
  0b00000000,
  0b00000000,
};
// uint32_t *init_pmem(uint32_t size){
//   uint32_t *memory=(uint32_t*)malloc(sizeof(uint32_t)*size);
//   memcpy(memory,img,sizeof(img));
//   return memory;
// }

void init_isa() {
  /* Load built-in image. */
  memcpy(guest_to_host(CONFIG_MBASE), img, sizeof(img));

  // /* Initialize this virtual computer system. */
  // restart();
}
