/* 
 * disk.c
 * Implements disk interaction functions 
 */

#include <stdio.h>
#include "../include/disk.h"
#include "../include/util.h"

FILE *disk_ptr = NULL;

void disk_init(const char *path) {
    // check if file exists using r+b
    disk_ptr = fopen(path, "r+b");
    if (disk_ptr == NULL) {
        // file doesn't exist, make it
        disk_ptr = fopen(path, "w+b");
        for (int i = 0; i < DISK_SIZE; i++) {
            fputc(0x00, disk_ptr);
        }
        printf("Made an empty virtual disk file. ");
    }else {
        printf("Found disk file: %s", path);
    }
}

void disk_read_block(int block, byte *buffer) {
    // set start of read to the block we want
    fseek(disk_ptr, block * BLOCK_SIZE, SEEK_SET);
    
    // block -> buffer
    fread(buffer, 1, BLOCK_SIZE, disk_ptr);
}

void disk_write_block(int block, byte *buffer) {
    // set start of write to the block we want
    fseek(disk_ptr, block * BLOCK_SIZE, SEEK_SET);

    // send buffer to block in blue.disk
    fwrite(buffer, 1, BLOCK_SIZE, disk_ptr);

    // write to file now, for safety
    fflush(disk_ptr);
}

void disk_close() {
    // write to file
    fclose(disk_ptr);
}