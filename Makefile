CC = gcc
SRC = src/main.c src/cpu.c src/disk.c
INCLUDE = -I include
OUT = blue-cpu.exe

ASSEMBLER = emulation/assembler.js


# Build blue-cpu.exe
all:
	$(CC) $(SRC) $(INCLUDE) -o $(OUT)

# Build assembler.exe
assemble:
	node $(ASSEMBLER) $(FILE)
	