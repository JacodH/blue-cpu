CC = gcc
SRC = src/main.c src/cpu.c src/disk.c src/screen.c
INCLUDE = -I include
OUT = blue-cpu.exe

ASSEMBLER = emulation/assembler.js

# Build blue-cpu.exe
all:
	$(CC) $(SRC) $(INCLUDE) -o $(OUT) -lSDL2 -lSDL2main

# Build assembly file using node.js
asm:
	node $(ASSEMBLER) $(FILE)