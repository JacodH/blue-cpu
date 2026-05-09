# *blue*-cpu
Custom CPU, ISA running on a custom C emulator made for research and practice in the C language. 

> Gif of the emulator running will go here

## Goal
The goal of Blue is to make a CPU / Emulator capable of running a custom OS. This would be the first time ive attempted to really learn more / make an operating system, so it's going to be crude. So the main goal of Blue is to have the CPU / Emulator ready for an OS when Purple comes. (The next CPU)

## Emulation Philosophy
No massive computation aside from pure emulation. ISA instructions should remain atomic and not "cheat". The emulator will not take any shortcuts that a real, physical CPU couldn't. 

## Table of Contents
1. [CPU docs](#cpu)
   1. [Notes](#notes)
   2. [Memory map](#memory-map)
   3. [Registers](#registers)
   4. [ISA](#isa)
      1. [Memory](#memory-instructions)
      2. [Arithmetic](#arithmetic-instructions)
      3. [Control](#control-instructions)
      4. [Stack](#stack-instructions)
      5. [Disk](#disk-instructions)
      6. [Emulation](#emulation-instructions)
2. [OS docs](#operating-system-lapse-10)
   1. [Notes](#notes-1)
   2. [File system](#file-system)
      1. [Disk layout](#disk-layout)
      2. [FAT table](#fat-table)
3. [Emulation](#emulation)
   1. [File structure]()
   2. [Screen]()
   3. [Disk]()

# CPU
### Notes 
 - 640 by 200 pixel screen (80 by 25 characters)
 - 64KB RAM (16 bit address space)
 - 16 registers 
 - 512KB disk
 - Little endian (low byte first)
 - Lapse OS
 - Kernel / User mode 
 - SYSCALL + SYSRET instruction
 - Interrupts (keyboard, timer, fault)

### Memory map 
| Region name  | Start    | Stop     | Size       | Purpose                                                      |
|--------------|----------|----------|------------|--------------------------------------------------------------|
| Boot jump    | `0x0000` | `0x0002` | 3 bytes    | Tells the CPU where to jump for OS boot / first instruction  |
| Vector table | `0x0003` | `0x0008` | 6 bytes    | Tells the CPU where to set the PC to on interrupt            |
| Font table   | `0x0009` | `0x0301` | 760 bytes  | Font table for VRAM (95 char * 8 bytes per char = 760 bytes) |
| Free RAM     | `0x0302` | `0xF830` | ~62KB      | Free RAM for programs / OS                                   |
| VRAM         | `0xF831` | `0xFFFF` | 2000 bytes | Memory mapped screen                                         |

The stack starts at `0xF830` (the top of free RAM) and grows downward. 

### Registers 
The CPU uses 16, 16 bit, general purpose registers that can be accessed at user level. The PC, SP, BASE and LIMIT registers are separate dedicated registers. 

BASE and LIMIT are privileged, meaning they can only be modified while in kernel mode. 

| Register | Purpose                                  |
|----------|------------------------------------------|
| r0       | Zero register                            |
| r1       | syscall number / return                  |
| r2       | syscall arg 1                            |
| r3       | syscall arg 2                            |
| r4       | syscall arg 3                            |
| r5 - r15 | General purpose                          |
| PC       | Program counter                          |
| SP       | Stack pointer                            |
| BASE     | Base of the allowed memory manipulation  | 
| LIMIT    | Limit of the allowed memory manipulation |


### ISA
All instructions are 4 bytes wide. The CPU is little endian, meaning low bytes go first, then high. 
***Memory check:*** Checks if kernel mode is true, if it is allow any address to change. If not, check to see if address is between BASE and LIMIT.
***Kernel required:*** This instruction can only be executed while the kernel mode is true. (Inside syscall)

#### Memory instructions
| Code | Description                                 | Privilege note | Name | a    | b       | c            | Pseudo                                                                           |
|------|---------------------------------------------|----------------|------|------|---------|--------------|---------------------------------------------------------------------------------|
| `0x01` | Sets register to immediate                  |                | SET  | rTAR | IMM_LOW | IMM_HGIH     | rTAR = (IMM_HIGH << 8) \| IMM_LOW                                               |
| `0x02` | Copies content from register to register    |                | MOV  | rSRC | rDST    |              | rDST = rSRC                                                                     |
| `0x03` | Clears content in register                  |                | CLR  | rTAR |         |              | rTAR = 0                                                                        |
| `0x04` | Loads RAM from register + offset            | Memory check   | GET  | rDST | rSRC    | IMM (signed) | rDST = RAM[rSRC+IMM] \| (RAM[rSRC+IMM+1] << 8)                                  |
| `0x05` | Store register into RAM to address + offset | Memory check   | STR  | rSRC | rDST    | IMM (signed) | RAM[rDST+IMM]   = rSRC & 0xFF (low byte) RAM[rDST+IMM+1] = rSRC >> 8 (high byte) |

#### Disk instructions
| Code | Description                                                  | Privilege note   | Name   | a           | b           | c      | Pseudo                                              |
|------|--------------------------------------------------------------|------------------|--------|-------------|-------------|--------|----------------------------------------------------|
| `0xd1` | Reads all memory from disk and sends it to RAM a RAM address | Kernel required  | DREAD  | rADDR       | rBLOCK     |        | RAM[rADDR] = disk[rBLOCK]                          |
| `0xd2` | Write memory from RAM to disk                                | Kernel required  | DWRITE | rADDR_START | rBLOCK      |        | disk[rBLOCK] = RAM[rADDR_START through rADDR_STOP] |

#### Arithmetic instructions
***(s)***: Signed value
Division by zero returns 0.
| Code | Description                            | Privilege note | Name | a    | b     | c     | Pseudo                     |
|------|----------------------------------------|----------------|------|------|-------|-------|----------------------------|
| 0xa1 | Adds two registeres                    |                | ADD  | rDST | rSRC1 | rSRC2 | rDST = rSRC1 + rSRC2       |
| 0xa2 | Subtracts two registers                |                | SUB  | rDST | rSRC1 | rSRC2 | rDST = rSRC1 - rSRC2       |
| 0xa3 | Multiplies two registers               |                | MUL  | rDST | rSRC1 | rSRC2 | rDST = rSRC1 * rSRC2       |
| 0xa4 | Divides two registers                  |                | DIV  | rDST | rSRC1 | rSRC2 | rDST = rSRC1 / rSRC2       |
| 0xa5 | Multiplies two signed registers        |                | SMUL | rDST | rSRC1 | rSRC2 | rDST = (s)rSRC1 * (s)rSRC2 |
| 0xa6 | Divides two signed registerse          |                | SDIV | rDST | rSRC1 | rSRC2 | rDST = (s)rSRC1 / (s)rSRC2 |
| 0xa7 | Adds an immediate to a register        |                | IADD | rDST | rSRC  | IMM   | rDST = rSRC + IMM          |
| 0xa8 | Subtracts an immediate from a register |                | ISUB | rDST | rSRC  | IMM   | rDST = rSRC - IMM          |

#### Control instructions
| Code | Description     | Privilege note | Name | a     | b    | c | Pseudo                         |
|------|-----------------|----------------|------|-------|------|---|-------------------------------|
| `0xC0` | Halts the CPU    |                | HLT  |          |      |   | Stops CPU                     |
| `0xC1` | No operation      |                | NOP  |          |      |   |                               |
| `0xC2` | Jump to address   |                | JMP  | rADDR    |      |   | PC = rADDR                    |
| `0xC3` | Jump to immediate |                | JMI  | IMM_ADDR |      |   | PC = IMM_ADDR                 |
| `0xC4` | Jump if true      |                | JIT  | rADDR    | rSRC |   | if (rSRC == true) PC = rADDR  |
| `0xC5` | Jump if false     |                | JIF  | rADDR    | rSRC |   | if (rSRC == false) PC = rADDR |

#### Stack instructions
#### Emulation instructions

# Operating System (***Lapse 1.0***) 
### Features
 - Memory protection
 - FAT like file system
 - Shell
 - Kernel / User mode


### File system
[FAT-8](https://en.wikipedia.org/wiki/File_Allocation_Table#Original_8-bit_FAT) like system. 
 - Disk Size: ***512 KB***
 - Block / Sector Size: ***512 bytes***
 - Max Files: ***32***
    - Max file name length: ***8 bytes***
    - Max file extension length: ***3 bytes***
    - Pointer to block: Since I have 1,024 blocks I need to represent 1,024 dif states so, ***2 bytes***
    - File Size: ***2 bytes***
 - Total Blocks: 524,288 bytes / 512 bytes per block = ***1,024***
 - File Directory Table: 32 files * (8 bytes + 3 bytes + 2 bytes + 2 bytes) = ***480 bytes*** 
 - FAT Table: 2 bytes per 1,024 blocks so ***2,048 bytes*** 
 - Data Blocks: The rest

##### Disk layout
| Region         | Start | Stop    | Size          |
|----------------|-------|---------|---------------|
| File directory | 0     | 479     | 480 bytes     |
| FAT table      | 480   | 2,527   | 2,048 bytes   |
| Data           | 2,528 | 524,287 | 521,760 bytes |

##### FAT Table
Every block has an entry in the FAT table, the entry is 2 bytes and tells the FS if a block is free, the end of the file, or pointing to another block. 
| FAT value     | Meaning               |
|---------------|-----------------------|
| 0x0000        | Free block            |
| 0xFFFF        | End of file           |
| Anything else | Pointer to next block |

# Emulation
Notes on how im emulating the CPU, Disk and screen in C. 

 - [ ] Make a 'cpu_memory_dump' feature to write CPU memory, disk and register data to file when called

### File structure

### Main loop

### Disk

### Screen