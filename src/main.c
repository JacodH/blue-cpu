/*
 * main.c
 * Setup and loop
 */

#include <stdio.h>
#include <string.h>

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
    // byte program[] = {
    //     0x01, 0x01, 0x20, 0x00,
    //     0x04, 0x02, 0x01, 0x00,
    //     0xb1, 0x03, 0x02, 0x00,
    //     0xc6, 0x03, 0x1c, 0x00,
    //     0xa7, 0x01, 0x01, 0x01,
    //     0xe1, 0x02, 0x00, 0x00,
    //     0xc3, 0x04, 0x00, 0x00,
    //     0xc0, 0x00, 0x00, 0x00,
    //     0x42, 0x65, 0x6c, 0x6c,
    //     0x6f, 0x21, 0x00, 0x00,
    //     0x00, 0x00,
    // };

    // load a compiled program 
    char loaded[] = "programs/hex/2.hex";
    printf("Loading from '%s'\n", loaded);
    
    byte program[65536]; // program buffer

    FILE *f = fopen(loaded, "rb");
    int size = fread(program, 1, 65536, f);
    fclose(f);

    // load cheat program
    for (int i = 0; i < size; i++) {
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
        cpu_execute(&cpu, opcode, a, b, c, false);

        // draw VRAM to screen
    }
    printf("\n");

    cpu_log_RAM(&cpu, 0x00d4, size, 'c');

    return 0;
}