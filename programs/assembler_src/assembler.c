#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include <string.h>

#include <stdbool.h>

int main(int argc, char *argv[]) {
    printf("Assembling program '%s' into executable machine code.\n", argv[1]);

    // get file pointer 
    char path[256];
    strcpy(path, "programs/");
    strcat(path, argv[1]);
    FILE *fptr = fopen(path, "r");

    if (fptr == NULL) {
        printf("Error: could not open file '%s'\n", path);
        return 1;
    }

    printf("Pass 0 - Cleaning and Labeling\n");
    // I want to go through every line and take out: 
    // 1. all comments 
    // 2. white space
    // 3. remove \n
    
    
    // split the file into 2 sections, data and text
    char text_lines[1024][256]; // 1024 lines, all 256 chars long
    int next_text_line = 0; // pointer to index

    char data_lines[1024][256]; 
    int next_data_line = 0;
    
    int line_count = 0; // just for display  

    char labels[256][32];     // 256 labels 32 chars in length
    uint16_t addresses[256];  // pointers for labels 
    int addr = 0; // for labels 


    // keeping track of section
    char section[] = "undefined";

    char line_buffer[256];
    while (fgets(line_buffer, 256, fptr)) {
        line_count++;
        // take out comments 
        char *semi = strchr(line_buffer, ';');
        // line has a comment 
        if (semi != NULL) {
            *semi = '\0'; // replace with terminator
        }

        // take out \n 
        char *newline = strchr(line_buffer, '\n');
        if (newline != NULL) {
            *newline = '\0'; // replace with terminator 
        }
        
        // white space check
        if (strspn(line_buffer, " \t\n\r") == strlen(line_buffer)) { 
            // line is white space
            continue;
        }

        // cutout spaces 
        int spaces = strspn(line_buffer, " ");
        char *cleaned = line_buffer + strspn(line_buffer, " \t");
        
        // Section detection 
        if (strcmp(cleaned, ".text") == 0) {
            printf("Start of .text section\n");
            strcpy(section, "text");
            continue;
        }
        if (strcmp(cleaned, ".data") == 0) {
            printf("Start of .data section\n");
            strcpy(section, "data");
            continue;
        }
        
        
        // Label detection
        char *colon = strchr(cleaned, ':');
        if (colon != NULL) {
            *colon = '\0';
            if (strcmp(section, "text") == 0) {
                printf("Found label in .text ['%s'] at address [0x%02x]\n", cleaned, addr);
                continue;
            }
            
            if (strcmp(section, "data") == 0) {
                printf("Found label in .data '%s'\n", cleaned);
                continue;
            }
        }
        
        
        if (strcmp(section, "text")) {
            // add this line to text_lines
            strcpy(text_lines[next_text_line], cleaned);
            next_text_line++;
        }
        
        if (strcmp(section, "data")) {
            // add this line to data_lines
            strcpy(data_lines[next_data_line], cleaned);
            next_data_line++;
        }
        
        printf("[%2d | 0x%04x]: %s\n", line_count, addr, cleaned);
        addr += 4;
    }
    
    return 0;
}