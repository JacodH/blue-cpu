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
2. [OS docs](#operating-system-lapse-10)
   1. [Notes](#notes-1)
   2. [File system](#file-system)
      1. [Disk layout](#disk-layout)
      2. [FAT table](#fat-table)

# CPU
### Notes 
 - Little endian (low byte first)
 - 640 by 200 pixel screen (80 by 25 characters)
 - 64KB RAM (16 bit address space)
 - 16 registers 
 - 512KB disk (virtual disk file)
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

```
PRIVILEGED: 
DREAD rADDR rBLOCK;     writes all bytes at rBLOCK to ram starting at rADDR
DWRITE rADDR rBLOCK;    writes all bytes at rADDR from ram to disk at rBLOCK
```

# Operating System (***Lapse 1.0***) 
### Notes
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