/*
 * main.c
 * Setup and loop
 */

#include <stdio.h>

#include "../include/util.h"
#include "../include/cpu.h"
#include "../include/disk.h"

int main() {
    printf("blue-cpu\n");

    struct CPU cpu;
    cpu_init(&cpu);
    disk_init("emulation/blue.disk");

    // cpu_log_RAM(&cpu, 0x0000, 0x000A);
    // cpu_log_registers(&cpu);

    cpu.RAM[0x0000] = 0x01; // SET
    cpu.RAM[0x0001] = 0x00; // r1
    cpu.RAM[0x0002] = 0x34; // 34 
    cpu.RAM[0x0003] = 0x12; // 12 

    // fetch decode execute loop
    while (cpu.running == true) {
        // fetch
        byte opcode = cpu_fetch(&cpu, cpu.PC);
        byte a = cpu_fetch(&cpu, cpu.PC+1);
        byte b = cpu_fetch(&cpu, cpu.PC+2);
        byte c = cpu_fetch(&cpu, cpu.PC+3);

        // decode and execute 
        cpu_execute(&cpu, opcode, a, b, c, true);

        // draw to screen
    }

    return 0;
}