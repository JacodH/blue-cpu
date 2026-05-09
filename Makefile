CC = gcc
SRC = src/main.c src/cpu.c src/disk.c
INCLUDE = -I include
OUT = blue-cpu.exe

ASSEMBLER = emulation/assembler/assemble.js

all:
	$(CC) $(SRC) $(INCLUDE) -o $(OUT)

asm: 
	node $(ASSEMBLER) $(FILE)