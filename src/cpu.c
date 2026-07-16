/*
 * cpu.c
 * Defines cpu functions. 
 */


#include <stdio.h>

#include "../include/util.h"
#include "../include/disk.h"
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
    printf("\nRAM cells 0x%04x through 0x%04x\n", start, end);
    for (int i = start; i <= end; i++) {
        printf("RAM[0x%04x] 0x%02x\n", i, (byte)cpu_ptr->RAM[i]);
    }
}

byte cpu_fetch(struct CPU *cpu_ptr, int addr) {
    return cpu_ptr->RAM[addr];
}

void cpu_execute(struct CPU *cpu_ptr, byte opcode, byte a, byte b, byte c, bool dev) {
    /*
     * The entire ISA needs to go here. Lots of coding to be done :)
     */
    if (dev) {printf("\n0x%04x [k=0] | ", cpu_ptr->PC);}
    switch (opcode) {
        // Memory instructions 
        case 0x01: // SET r1-16 immediate
            if (dev) {printf("SET r%d 0x%04x", a, (c << 8) | b);}
            cpu_ptr->registers[a] = (c << 8) | b;
            cpu_ptr->PC += 4;
            break;
        case 0x02: // MOV r1-16 r1-16
            if (dev) {printf("MOV r%d r%d", a, b);}
            cpu_ptr->registers[b] = cpu_ptr->registers[a];
            cpu_ptr->PC += 4;
            break;
        case 0x03: // CLR r1-16
            if (dev) {printf("CLR r%d", a);}
            cpu_ptr->registers[a] = 0;
            cpu_ptr->PC += 4;
            break;
        case 0x04: { // GET r1-16 r1-16 IMM
            if (dev) {printf("GET r%d RAM[0x%02x] (0x%02x, %d)", a, b, (sbyte)c, (sbyte)c);}
            // TODO: Memory protection
            word addr = cpu_ptr->registers[b] + (sbyte)c;
            cpu_ptr->registers[a] = cpu_ptr->RAM[addr] | (cpu_ptr->RAM[addr + 1] << 8);
            cpu_ptr->PC += 4;
            break;
        }
        case 0x05: { // STR r1-16 r1-16 IMM
            if (dev) {printf("STR r%d RAM[0x%02x] (0x%02x, %d)", a, b, (sbyte)c, (sbyte)c);}
            // TODO: Memory protection 
            word addr = cpu_ptr->registers[b] + (sbyte)c;
            
            // low byte
            cpu_ptr->RAM[addr+0] = cpu_ptr->registers[a] & 0xFF;
            // high byte
            cpu_ptr->RAM[addr+1] = cpu_ptr->registers[a] >> 8;
            cpu_ptr->PC += 4;
            break;
        }

        // Disk instructions 
        case 0xd1: { // DREAD r1-16 r1-16
            if (dev) {printf("DREAD RAM[%d] r%d", a, b);}

            // Make buffer for disk read
            byte buff[BLOCK_SIZE];

            // Set buffer to content at block b
            disk_read_block(cpu_ptr->registers[b], buff);

            // Set RAM to buffer
            for (int i = 0; i < BLOCK_SIZE; i++) {
                cpu_ptr->RAM[i+cpu_ptr->registers[a]] = buff[i];
            }

            cpu_ptr->PC += 4;
            break;
        }

        // Arithmetic instructions
        case 0xa1: { // ADD r1-16 r1-16 r1-16
            if (dev) {printf("ADD r%d r%d r%d", a, b, c);};
            cpu_ptr->registers[a] = cpu_ptr->registers[b] + cpu_ptr->registers[c];
            cpu_ptr->PC += 4;
            break;
        }
        case 0xa2: { // SUB r1-16 r1-16 r1-16
            if (dev) {printf("SUB r%d r%d r%d", a, b, c);};
            cpu_ptr->registers[a] = cpu_ptr->registers[b] - cpu_ptr->registers[c];
            cpu_ptr->PC += 4;
            break;
        }
        case 0xa3: { // MUL r1-16 r1-16 r1-16
            if (dev) {printf("MUL r%d r%d r%d", a, b, c);};
            cpu_ptr->registers[a] = cpu_ptr->registers[b] * cpu_ptr->registers[c];
            cpu_ptr->PC += 4;
            break;
        }
        case 0xa4: { // DIV r1-16 r1-16 r1-16
            if (dev) {printf("DIV r%d r%d r%d", a, b, c);};
            if (cpu_ptr->registers[c] == 0) {
                // Can't divide by 0
                cpu_ptr->registers[a] = 0;
                cpu_ptr->PC += 4;
                break;
            }else {
                cpu_ptr->registers[a] = cpu_ptr->registers[b] / cpu_ptr->registers[c];
                cpu_ptr->PC += 4;
                break;
            }
        }
        case 0xa5: { // SMUL r1-16 r1-16 r1-16
            if (dev) {printf("SMUL r%d r%d r%d", a, b, c);};
            cpu_ptr->registers[a] = (sword)cpu_ptr->registers[b] * (sword)cpu_ptr->registers[c];
            cpu_ptr->PC += 4;
            break;
        }
        case 0xa6: { // SDIV r1-16 r1-16 r1-16
            if (dev) {printf("SDIV r%d r%d r%d", a, b, c);};
            if (cpu_ptr->registers[c] == 0) {
                // Can't divide by 0
                cpu_ptr->registers[a] = 0;
                cpu_ptr->PC += 4;
                break;
            }else {
                cpu_ptr->registers[a] = (sword)cpu_ptr->registers[b] / (sword)cpu_ptr->registers[c];
                cpu_ptr->PC += 4;
                break;
            }
        }
        case 0xa7: { // IADD r1-16 r1-16 IMM
            if (dev) {printf("IADD r%d r%d (0x%02x, %d)", a, b, c, c);};
            cpu_ptr->registers[a] = (sword)cpu_ptr->registers[b] + (sbyte)c;
            cpu_ptr->PC += 4;
            break;
        }
        case 0xa8: { // ISUB r1-16 r1-16 IMM
            if (dev) {printf("ISUB r%d r%d (0x%02x, %d)", a, b, c, c);};
            cpu_ptr->registers[a] = (sword)cpu_ptr->registers[b] - (sbyte)c;
            cpu_ptr->PC += 4;
            break;
        }

        // Comparison instructions
        case 0xb1: {
            if (dev) {printf("EQ  r%d r%d r%d", a, b, c);};
            cpu_ptr->registers[a] = (cpu_ptr->registers[b] == cpu_ptr->registers[c]);
            cpu_ptr->PC += 4;
            break;
        }


        // Control instructions 
        case 0xc0: // HLT;
            if (dev) {printf("HLT");}
            cpu_ptr->running = false;
            break;

        case 0xc1: // NOP;
            if (dev) {printf("NOP");}
            cpu_ptr->PC += 4;
            break;
        case 0xc2: // JMP r1-16
            if (dev) {printf("JMP r%d", a);};
            cpu_ptr->PC = cpu_ptr->registers[a];
            break;
        case 0xc3: // IJMP IMM
            if (dev) {printf("IJMP (0x%02x, %d)", a, a);};
            cpu_ptr->PC = a;
            break;
        
        case 0xc6: { // IJIT rSRC IMM_LOW IMM_HIGH
            if (dev) {printf("IJIT r%d 0x%02x 0x%02x", a, b, c);};
            word addr = (c << 8) | b;
            if (cpu_ptr->registers[a] == 1) {
                cpu_ptr->PC = addr;
            }else {
                cpu_ptr->PC+=4;
            }
            break;
        }

        case 0xc7: { // IJIF rSRC IMM_LOW IMM_HIGH
            if (dev) {printf("IJIF r%d 0x%02x 0x%02x", a, b, c);};
            word addr = (c << 8) | b;
            if (cpu_ptr->registers[a] == 0) {
                cpu_ptr->PC = addr;
            }else {
                cpu_ptr->PC+=4;
            }
            break;
        }

        // Emulation instructions 
        case 0xe1: // OUT r1-16
            if (dev == false) {printf("\n");};
            printf("OUT r%d = [0x%04x, %d, %c]", a, cpu_ptr->registers[a], cpu_ptr->registers[a], cpu_ptr->registers[a]); 
            cpu_ptr->PC += 4;
            break;

        case 0xe2: // SOUT r1-16
            if (dev == false) {printf("\n");};
            printf("SOUT r%d = [0x%04x, %d]", a, (word)cpu_ptr->registers[a], (sword)cpu_ptr->registers[a]); 
            cpu_ptr->PC += 4;
            break;

        default: 
            printf("\nUnknown opcode '0x%02x'\n", opcode);
            cpu_ptr->running = false;

    }
}