/*
 * main.c
 * Setup and loop
 */

#include <stdio.h>
#include <string.h>

#include "../include/screen.h"
#include "../include/util.h"
#include "../include/disk.h"
#include "../include/cpu.h"

#undef main

int main() {
    printf("blue-cpu\n");


    // init cpu emulator
    struct CPU cpu;
    cpu_init(&cpu);
    disk_init("emulation/blue.disk");

    // cpu_log_RAM(&cpu, 0x0000, 0x000A);
    // cpu_log_registers(&cpu);

    // init emulator screen
    struct Screen screen = create_screen(8*25, 8*25, 2);

    // load a compiled program from file
    char loaded[] = "programs/hex/3.hex";
    printf("Loading from '%s'\n", loaded);
    
    byte program[65536]; // program buffer

    FILE *f = fopen(loaded, "rb");
    int size = fread(program, 1, 65536, f);
    fclose(f);

    // load cheat program
    for (int i = 0; i < size; i++) {
        cpu.RAM[i] = program[i];
    }

    int frame = 0;

    printf("Starting execution\n");
    SDL_SetWindowTitle(screen.window, "blue-cpu | running");
    // fetch decode execute loop
    while (cpu.running == true) {
        // check to see if user tried to close the CPU
        check_close(&screen);

        // fetch
        byte opcode = cpu_fetch(&cpu, cpu.PC);
        byte a = cpu_fetch(&cpu, cpu.PC+1);
        byte b = cpu_fetch(&cpu, cpu.PC+2);
        byte c = cpu_fetch(&cpu, cpu.PC+3);

        // decode and execute 
        cpu_execute(&cpu, opcode, a, b, c, false);

        // only render screen every 100 instructions 
        frame++;
        if (frame >= 100) {
            frame = 0;
            // clear the screen
            clear_screen(&screen);
            // draw vram
            draw_vram(&screen, &cpu);
            // update buffer
            SDL_RenderPresent(screen.renderer);
        }
        // SDL_Delay(1);
    }
    printf("Finished execution\n");
    SDL_SetWindowTitle(screen.window, "blue-cpu | halted");

    cpu_log_RAM(&cpu, VRAM_START, VRAM_END, 'c');

    while (1) {
        // check if user closed emulator 
        check_close(&screen);

        // clear the screen
        clear_screen(&screen);
        
        // draw vram
        draw_vram(&screen, &cpu);
        
        // update buffer
        SDL_RenderPresent(screen.renderer);
    
        SDL_Delay(16);
    }
    return 0;
}