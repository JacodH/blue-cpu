CC = gcc
SRC = src/main.c src/cpu.c src/disk.c
INCLUDE = -I include
OUT = blue-cpu.exe

all:
	$(CC) $(SRC) $(INCLUDE) -o $(OUT)