/*
 * util.h
 * Just defines some useful variables for the CPU and emulation. 
 */

#include <stdint.h>

#define RAM_SIZE 65536     // 64 kb

#define DISK_SIZE 524288   // 512 kb
#define BLOCK_SIZE 512     // 512 bytes

// I love C so much 
typedef uint8_t byte;
typedef uint16_t word;

#define bool int
#define true 1
#define false 0