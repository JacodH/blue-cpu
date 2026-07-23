; Conway's game of life.
; Rules
;   1. Any alive cell with fewer than 2 live neighbors dies
;   2. Any alive cell with two or three live neighbors lives
;   3. Any alive cell with more than 3 live neighbors dies
;   4. Any dead cell with exactly three live neighbors becomes alive
;
; Rules in pseudo
;   n = (int)alive_neighbors 
;   a = (boolean)alive
;
;   ; inside loop over all cells
;   ; must output to a buffer
;   if (a == true) {
;       if (n < 2) { die(); } ; rule 1
;       if (n == 2 || n == 3) { live(); } ; rule 2
;       if (n > 3) { die(); } ; rule 3
;       break;
;   }else {
;       if (n == 3) { born(); } ; rule 4
;   }
;
; Seems pretty simple
;
; I need to figure out way to store the array. 
; 1. 2 vram buffers
;   The easiest way would be to have 2 buffers the size of my vram
;   But my vram is 625 bytes.. thats 1.2 kilobytes just for this one program
; 2. Compressed buffers
;   since each tile in the game is only alive or dead (one bit)
;   i could compress the tiles such that i can store 8 tiles in 1 byte
;   if i want the game to be 25x25 tiles (to match the screen)
;   then i need...
;   ((25*25) / 8) * 2 = 158 bytes. 
;   thats including there being 2 buffers. I gotta try this.
;
; This will make it a little harder to actually compute the game..
; I'd have to kinda solo the lookup table.
; Could be fun and its MUCH more memory efficient 
;
; Alright now that i know how the data will be stored, how should i start?
; [ ] Writing buffer to screen
;       To make sure its working, ill encode some kinda pattern into the bitmap
; [ ] Clearing buffer function 
; [ ] grow() die() live() functions
; [ ] testing for all of that
; [ ] looping through all the cells 
; [ ] implementing the rules
; [ ] done? 

.text
render_buffer:
    ; loop through every byte in buffer array
    ; increments of 8
    ; increment VRAM pointer
    ; set vram
    ; return

clear_buffer:
    ; clears buffer being pointed at by r1
    ; once i > buff_size return

.data
    ; Need 2 157 byte buffers
    ; test: 0x00 0x15 0x15 0x61
    buff1: 0x92 * 20