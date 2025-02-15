#ifndef _COMMON_H_
#define _COMMON_H_
#include <stdint.h>
#include <inttypes.h>
#include <stdbool.h>
#include <string.h>
#include<stdint.h>
#include <macro.h>
#define CONFIG_MBASE 0x80000000
typedef uint32_t paddr_t;
#define FMT_WORD MUXDEF(CONFIG_ISA64, "0x%016" PRIx64, "0x%08" PRIx32)
#endif