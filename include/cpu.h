/*
 * cpu.h
 * Declares up cpu functions. 
 */

#include <stdint.h>

struct CPU {
    byte RAM[RAM_SIZE];   // 64 KB RAM 
    word registers[16];   // 16 16 bit registers
    bool kernel;          // Kernel bit (1 = kernel mode 0 = user mode)

    word PC;              // Program counter
    word SP;              // Stack counter
    word BASE;            // memory protection base
    word LIMIT;           // memory protection limit

    // emulation 
    bool running;
};

void cpu_init(struct CPU *cpu_ptr);

void cpu_log_registers(struct CPU *cpu_ptr);
void cpu_log_RAM(struct CPU *cpu_ptr, word start, word end);

// returns byte at memory addr
byte cpu_fetch(struct CPU *cpu_ptr, int addr);

// decodes and executes instruction
void cpu_execute(struct CPU *cpu_ptr, byte opcode, byte a, byte b, byte c, bool dev);
