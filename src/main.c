/*
 * main.c
 * Setup and loop
 */

#include <stdio.h>

#include "../include/util.h"
#include "../include/disk.h"
#include "../include/cpu.h"

int main() {
    printf("blue-cpu\n");

    struct CPU cpu;
    cpu_init(&cpu);
    disk_init("emulation/blue.disk");

    // cpu_log_RAM(&cpu, 0x0000, 0x000A);
    // cpu_log_registers(&cpu);

    // cheat program loading for testing 
    byte program[] = {
        0x01, 0x00, 0x00, 0x00,
        0xa7, 0x00, 0x00, 0x01,
        0xe1, 0x00, 0x00, 0x00,
        0xc3, 0x04, 0x00, 0x00,
    };
    int program_bytes = sizeof(program);

    // load cheat program
    for (int i = 0; i < program_bytes; i++) {
        cpu.RAM[i] = program[i];
    }

    printf("Starting execution\n");
    // fetch decode execute loop
    while (cpu.running == true) {
        // fetch
        byte opcode = cpu_fetch(&cpu, cpu.PC);
        byte a = cpu_fetch(&cpu, cpu.PC+1);
        byte b = cpu_fetch(&cpu, cpu.PC+2);
        byte c = cpu_fetch(&cpu, cpu.PC+3);

        // decode and execute 
        cpu_execute(&cpu, opcode, a, b, c, true);

        // draw VRAM to screen
    }
    printf("\n");

    // cpu_log_RAM(&cpu, 0x8000, 0x8001);

    return 0;
}