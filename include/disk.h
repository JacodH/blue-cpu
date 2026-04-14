/*
 * disk.h
 * Defines disk interaction functions
 */

#include "util.h"

void disk_init(const char *path);
void disk_read_block(int block, byte *buffer);
void disk_write_block(int block, byte *buffer);
void disk_close();