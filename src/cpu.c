/*
 * cpu.c
 * Defines cpu functions. 
 */


#include <stdio.h>

#include "../include/util.h"
#include "../include/cpu.h"

void cpu_init(struct CPU *cpu_ptr) {
    // clear RAM
    for (int i = 0; i < RAM_SIZE; i++) {
        cpu_ptr->RAM[i] = 0x00;
    }

    // clear registers
    for (int i = 0; i < 16; i++) {
        cpu_ptr->registers[i] = 0x00;
    }

    // clear PC
    cpu_ptr->PC = 0x00;

    // clear SP
    cpu_ptr->SP = 0x00;

    // clear BASE
    cpu_ptr->BASE = 0x00;
    
    // clear LIMIT
    cpu_ptr->LIMIT = 0x00;

    // set running to true
    cpu_ptr->running = true;
}

void cpu_log_registers(struct CPU *cpu_ptr) {
    printf("Registers: ");
    for (int i = 0; i < 16; i++) {
        if (i % 4 == 0) {printf("\n");}
        printf("[%c 0x%04x]", i+0x41, cpu_ptr->registers[i]);
    }
}

void cpu_log_RAM(struct CPU *cpu_ptr, word start, word end) {
    printf("RAM cells 0x%02x through 0x%02x\n", start, end);
    for (int i = start; i <= end; i++) {
        printf("0x%02x [0x%04x]\n", i, cpu_ptr->RAM[i]);
    }
}

byte cpu_fetch(struct CPU *cpu_ptr, int addr) {
    return cpu_ptr->RAM[addr];
}

void cpu_execute(struct CPU *cpu_ptr, byte opcode, byte a, byte b, byte c, bool dev) {
    /*
     * The entire ISA needs to go here. Lots of coding to be done :)
     */
    if (dev) {printf("\n0x%04x | ", cpu_ptr->PC);}
    switch (opcode) {
        // Memory
        case 0x01: // SET r1-16 immediate
            if (dev) {printf("SET r%d 0x%04x", a, (c << 8) | b);}
            cpu_ptr->registers[a] = (c << 8) | b;
            cpu_ptr->PC += 4;
            break;
        default: 
            printf("Unknown opcode '0x%02x'", opcode);
            cpu_ptr->running = false;
    }
}