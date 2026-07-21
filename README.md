# \_blue\_-cpu

Custom CPU and ISA running on a emulator made for research and practice in the C language.

> Gif of the emulator running will go here

## Goal

The goal of Blue is to make a CPU / Emulator capable of running a custom OS. This would be the first time ive attempted to really learn more / make an operating system, so it's going to be crude. So the main goal of Blue is to have the CPU / Emulator ready for an OS when Purple comes. (The next CPU)

## Emulation Philosophy

No massive computation aside from pure emulation. ISA instructions should remain atomic and not "cheat". The emulator will not take any shortcuts that a real, physical CPU couldn't.

## Table of Contents

1.  [CPU docs](#cpu)
    1.  [Notes](#notes)
    2.  [Memory map](#memory-map)
    3.  [Registers](#registers)
    4.  [ISA](#isa)
        1.  [Memory](#memory-instructions)
        2.  [Arithmetic](#arithmetic-instructions)
        3.  [Control](#control-instructions)
        4.  [Stack](#stack-instructions)
        5.  [Disk](#disk-instructions)
        6.  [Emulation](#emulation-instructions)
2.  [OS docs](#operating-system-lapse-10)
    1.  [Notes](#notes-1)
    2.  [File system](#file-system)
        1.  [Disk layout](#disk-layout)
        2.  [FAT table](#fat-table)
3.  [Emulation](#emulation)
    1.  File structure
    2.  Screen
    3.  Disk

# CPU

### Notes

*   200 by 200 pixel screen (25\*5 by 25\*8 characters, window is also scaled up)
    *   625 byte VRAM
*   64KB RAM (16 bit address space)
*   16 registers
*   512KB disk
*   Little endian (low byte first)
*   Lapse OS
    *   Kernel / User mode
    *   SYSCALL + SYSRET instruction
    *   Interrupts (keyboard, timer, fault)

### Memory map

| Region name | Start | Stop | Size | Purpose |
| --- | --- | --- | --- | --- |
| Free RAM | `0x0000` | `0xFB98` | 64,409 bytes | Free RAM for programs / OS |
| Programs stack | ~0xFB99 | 0xFD8D | 500 bytes (truly variable) | Stack for programs, grows downward. Well give it like 500 bytes |
| VRAM | `0xFD8E` | `0xFFFF` | 625 bytes | Memory mapped screen |

The stack starts at `0xF830` (the top of free RAM) and grows downward.

### Registers

The CPU uses 16, 16 bit, general purpose registers that can be accessed at user level. The PC, SP, BASE and LIMIT registers are separate dedicated registers.

BASE and LIMIT are privileged, meaning they can only be modified while in kernel mode.

| Register | Purpose |
| --- | --- |
| r0 | Zero register |
| r1 | syscall number / return |
| r2 | syscall arg 1 |
| r3 | syscall arg 2 |
| r4 | syscall arg 3 |
| r5 - r15 | General purpose |
| PC | Program counter |
| SP | Stack pointer |
| BASE | Base of the allowed memory manipulation |
| LIMIT | Limit of the allowed memory manipulation |

### ISA

All instructions are 4 bytes wide. The CPU is little endian, meaning low bytes go first, then high.  
_**Memory check:**_ Checks if kernel mode is true, if it is allow any address to change. If not, check to see if address is between BASE and LIMIT.  
_**Kernel required:**_ This instruction can only be executed while the kernel mode is true. (Inside syscall)

#### Memory instructions

| Code | Description | Privilege note | Name | a | b | c | Pseudo |
| --- | --- | --- | --- | --- | --- | --- | --- |
| `0x01` | Sets register to immediate |   | SET | rTAR | IMM\_LOW | IMM\_HIGH | rTAR = (IMM\_HIGH \<\< 8) |
| `0x02` | Copies content from register to register |   | MOV | rSRC | rDST |   | rDST = rSRC |
| `0x03` | Clears content in register |   | CLR | rTAR |   |   | rTAR = 0 |
| `0x04` | Loads RAM from register + offset | Memory check | GET | rDST | rSRC | IMM (signed) | rDST = RAM\[rSRC+IMM\] |
| `0x05` | Store register into RAM to address + offset | Memory check | STR | rSRC | rDST | IMM (signed) | RAM\[rDST+IMM\] = rSRC & 0xFF (low byte) RAM\[rDST+IMM+1\] = rSRC >> 8 (high byte) |

#### Disk instructions

| Code | Description | Privilege note | Name | a | b | c | Pseudo |
| --- | --- | --- | --- | --- | --- | --- | --- |
| `0xd1` | Reads all memory from disk and sends it to RAM a RAM address | Kernel required | DREAD | rADDR | rBLOCK |   | RAM\[rADDR\] = disk\[rBLOCK\] |
| `0xd2` | Write memory from RAM to disk | Kernel required | DWRITE | rADDR\_START | rBLOCK |   | disk\[rBLOCK\] = RAM\[rADDR\_START through rADDR\_STOP\] |

#### Arithmetic instructions

_**(s)**_: Signed value  
Division by zero returns 0.

| Code | Description | Privilege note | Name | a | b | c | Pseudo |
| --- | --- | --- | --- | --- | --- | --- | --- |
| 0xa1 | Adds two registeres |   | ADD | rDST | rSRC1 | rSRC2 | rDST = rSRC1 + rSRC2 |
| 0xa2 | Subtracts two registers |   | SUB | rDST | rSRC1 | rSRC2 | rDST = rSRC1 - rSRC2 |
| 0xa3 | Multiplies two registers |   | MUL | rDST | rSRC1 | rSRC2 | rDST = rSRC1 \* rSRC2 |
| 0xa4 | Divides two registers |   | DIV | rDST | rSRC1 | rSRC2 | rDST = rSRC1 / rSRC2 |
| 0xa5 | Multiplies two signed registers |   | SMUL | rDST | rSRC1 | rSRC2 | rDST = (s)rSRC1 \* (s)rSRC2 |
| 0xa6 | Divides two signed registerse |   | SDIV | rDST | rSRC1 | rSRC2 | rDST = (s)rSRC1 / (s)rSRC2 |
| 0xa7 | Adds an immediate to a register |   | IADD | rDST | rSRC | IMM | rDST = rSRC + IMM |
| 0xa8 | Subtracts an immediate from a register |   | ISUB | rDST | rSRC | IMM | rDST = rSRC - IMM |

#### Comparison instructions

| Code | Description | Privilege note | Name | a | b | c | Pseudo |
| --- | --- | --- | --- | --- | --- | --- | --- |
| 0xb1 | Compares equality of registers |   | EQ | rDST | rSRC1 | rSRC2 | rDST = rSRC1 == rSRC2 |
| 0xb2 | Compares inequality of registers |   | NE | rDST | rSRC1 | rSRC2 | rDST = rSRC1 != rSRC2 |
| 0xb3 | Less than comparison |   | LT | rDST | rSRC1 | rSRC2 | rDST = rSRC1 \< rSRC2 |
| 0xb4 | Greater than comparison |   | GT | rDST | rSRC1 | rSRC2 | rDST = rSRC1 > rSRC2 |
| 0xb5 | Less than comparison signed |   | LTS | rDST | rSRC1 | rSRC2 | rDST = (s)rSRC1 \< (s)rSRC2 |
| 0xb6 | Greater than comparison signed |   | GTS | rDST | rSRC1 | rSRC2 | rDST = (s)rSRC1 > (s)rSRC2 |

#### Bitwise instructions

<table><tbody><tr><td>Code</td><td>Description</td><td>Privilege note</td><td>Name</td><td>a</td><td>b</td><td>c</td><td>Pseudo</td></tr><tr><td>0x81</td><td>&nbsp;</td><td>&nbsp;</td><td>AND</td><td>rDST</td><td>rSRC1</td><td>rSRC2</td><td>rDST = rSRC1 &amp; rSRC2</td></tr><tr><td>0x82</td><td>&nbsp;</td><td>&nbsp;</td><td>XOR</td><td>rDST</td><td>rSRC1</td><td>rSRC2</td><td>rDST = rSRC1 ^ rSRC2</td></tr><tr><td>0x83</td><td>&nbsp;</td><td>&nbsp;</td><td>LSH</td><td>rDST</td><td>rSRC1</td><td>rSRC2</td><td>rDST = rSRC1 &lt;&lt; rSRC2</td></tr><tr><td>0x84</td><td>&nbsp;</td><td>&nbsp;</td><td>RSH</td><td>rDST</td><td>rSRC1</td><td>rSRC2</td><td>rDST = rSRC1 &gt;&gt; rSRC2</td></tr></tbody></table>

#### Control instructions

| Code | Description | Privilege note | Name | a | b | c | Pseudo |
| --- | --- | --- | --- | --- | --- | --- | --- |
| `0xC0` | Halts the CPU |   | HLT |   |   |   | Stops CPU |
| `0xC1` | No operation |   | NOP |   |   |   |   |
| `0xC2` | Jump to address |   | JMP | rADDR |   |   | PC = rADDR |
| `0xC3` | Jump to immediate |   | IJMP | IMM\_ADDR |   |   | PC = IMM\_ADDR |
| `0xC4` | Jump if true |   | JIT | rADDR | rSRC |   | if (rSRC == true) PC = rADDR |
| `0xC5` | Jump if false |   | JIF | rADDR | rSRC |   | if (rSRC == false) PC = rADDR |
| `0xc6` | Immediate jump if true |   | IJIT | rSRC | IMM\_ADDR | IMM\_ADDR | if (rSRC == true) PC = IMM\_ADDR |
| `0xc7` | Immediate jump if flase |   | IJIF | rSRC | IMM\_ADDR | IMM\_ADDR | if (rSRC == false) PC = IMM\_ADDR |

### Stack instructions

| Code | Description | Privilege note | Name | a | b | c | Pseudo |
| --- | --- | --- | --- | --- | --- | --- | --- |
| `0xf1` | Pushes register to the stack |   | PUSH | rSRC |   |   | SP -= 2; RAM\[SP\] = rSRC & 0xFF; RAM\[SP+1\] = rSRC >> 8 |
| `0xf2` | Pops to register from the stack |   | POP | rSRC |   |   | rSRC = RAM\[SP\] |
| `0xf3` | Pushes PC to stack, jumps to immediate |   | CALL | IMM\_LOW | IMM\_HIGH |   | SP-=2; RAM\[SP\] = PC; PC = IMM\_LOW |
| `0xf4` | Pops PC from the stack, jumps to it |   | RET |   |   |   | PC = RAM\[SP\] |

#### Emulation instructions

# Operating System (_**Lapse 1.0**_)

### Features

*   Memory protection
*   FAT like file system
*   Shell
*   Kernel / User mode

### File system

[FAT-8](https://en.wikipedia.org/wiki/File_Allocation_Table#Original_8-bit_FAT) like system.

*   Disk Size: _**512 KB**_
*   Block / Sector Size: _**512 bytes**_
*   Max Files: _**32**_
    *   Max file name length: _**8 bytes**_
    *   Max file extension length: _**3 bytes**_
    *   Pointer to block: Since I have 1,024 blocks I need to represent 1,024 dif states so, _**2 bytes**_
    *   File Size: _**2 bytes**_
*   Total Blocks: 524,288 bytes / 512 bytes per block = _**1,024**_
*   File Directory Table: 32 files \* (8 bytes + 3 bytes + 2 bytes + 2 bytes) = _**480 bytes**_
*   FAT Table: 2 bytes per 1,024 blocks so _**2,048 bytes**_
*   Data Blocks: The rest

##### Disk layout

| Region | Start | Stop | Size |
| --- | --- | --- | --- |
| File directory | 0 | 479 | 480 bytes |
| FAT table | 480 | 2,527 | 2,048 bytes |
| Data | 2,528 | 524,287 | 521,760 bytes |

##### FAT Table

Every block has an entry in the FAT table, the entry is 2 bytes and tells the FS if a block is free, the end of the file, or pointing to another block.

| FAT value | Meaning |
| --- | --- |
| 0x0000 | Free block |
| 0xFFFF | End of file |
| Anything else | Pointer to next block |

# Emulation

Notes on how im emulating the CPU, Disk and screen in C.

*   Make a 'cpu\_memory\_dump' feature to write CPU memory, disk and register data to file when called

### File structure

### Main loop

### Disk

### Screen