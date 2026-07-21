/*
 * util.h
 * Just defines some useful variables for the CPU and emulation. 
 */

#include <stdint.h>

#define RAM_SIZE 65536     // 64 kb

#define DISK_SIZE 524288   // 512 kb
#define BLOCK_SIZE 512     // 512 bytes

#define VRAM_START 0xF9DA  // where vram starts = 0xffff - (25*25) 
#define VRAM_END 0xffff

// I love C so much 
typedef uint8_t byte;
typedef uint16_t word;

typedef int8_t sbyte;
typedef int16_t sword;

#define bool int
#define true 1
#define false 0