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
;   ((25*25) / 8) = 79 bytes.
;   for 2 buffers thats 157. 
; 
;   I gotta try this.
;
; This will make it a little harder to actually compute the game..
; I'd have to kinda solo the lookup table.
; Could be fun and its MUCH more memory efficient 
;
; Alright now that i know how the data will be stored, how should i start?
; [x] Writing buffer to screen
;       To make sure its working, ill encode some kinda pattern into the bitmap
; [x] Clearing buffer function 
; [x] move functions for moving buff1 to buff2 and vise versa 
; [ ] grow() die() live() functions
;       If i wanna do this i gotta actually make a formula to go from x and y to addr and bit shift
;       ill call it get_addr
;       then i needa actually make the functions
;       they just need to take an x and y then flip that bit at the correct addr+offset
; [ ] testing for all of that
; [ ] looping through all the cells 
; [ ] implementing the rules
; [ ] done? 

.text
main:
    ; loop
        ; move buff2 -> buff1
        ; clear buff2
        ; compute game, output goes to buff2
        ; jump to main

    ; SET r1 24;
    ; SET r2 24;
    ; CALL $get_addr;
    ; OUT r1;
    ; OUT r2;
    SET r3 @buff1;

    SET r1 0;
    SET r2 0; 
    CALL $grow;

    SET r1 24;
    SET r2 0; 
    CALL $grow;

    SET r1 0;
    SET r2 24; 
    CALL $grow;

    SET r1 24;
    SET r2 24; 
    CALL $grow;

    ; render buffer 1
    CALL $render_buffer;

    HLT;

; Notes
; This function takes in an x and y screen coord and returns a 
; memory address and a bit shift offset

; can i just compute like normal then div by 8? yes. then give the remainder to r2 as bit shift
; 
; i = (y*width) + x
; byte = i / 8
; shl = i % 8 

; Parameters
    ; r1 = x cord
    ; r2 = y cord
; Returns
    ; r1 = byte 
    ; r2 = left shifts
; Variables
    ; r3 = x cord copy
    ; r4 = y cord copy
    ; r5 = 25 (width const)
    ; r6 = i
get_addr: 
    MOV r1 r3; r3 = x cord
    MOV r2 r4; r4 = y cord

    SET r5 25; ; r5 = width

    MUL r6 r4 r5; r6 = y * 25
    ADD r6 r6 r3; r6 = (y*25) + x

    SET r7 8; r7 = 8;

    DIV r1 r6 r7; r1 = index / 8
    MOD r2 r6 r7; r2 = index % 8
    RET;


; Notes
; takes in an x and y cord and sets that bit to 1
; Parameters: 
; r1 = x
; r2 = y
; r3 = buffer_ptr
; Variables
; r1 = byte offset
; r2 = bit offset
; r3 = buffer_ptr
; r4 = bit_replacer
; r5 = byte holder
; Pseudo 
; byte_offset = get_addr(x, y)[0]
; bit_offset = get_addr(x, y)[1]
; byte_replacement = byte | (0x01 << bit_offset)
; RAM[byte_offset+buffer_ptr] = byte_replacement
grow: 
    PUSH r3;
    ; r1 = x
    ; r2 = y
    ; r3 = buffer_ptr
    CALL $get_addr;
    ; r1 = byte_offset
    ; r2 = bit_offset

    POP r3;

    ; bit_replacer
    SET r4 0x0001; 0b0000000000000001
    LSH r4 r4 r2; r4 = r4 << bit_offset

    ; add buffer pointer
    ADD r1 r1 r3; 

    ; get current byte
    GETB r5 r1 0;

    ; do OR operation 
    OR r5 r5 r4; r5 = current_byte | bit_replacer

    ; store back into ram
    STRB r5 r1 0;
    RET;

; Notes
; This function will only ever print buff1
; can be changed 
; Parameters
; 
; Variables
; r1 = loop counter
; r2 = bit offset counter
; r3 = VRAM pointer
; r4 = tile value
; r5 = byte holder
; r6 = tool to single out least sig bit

; r12 = buffer pointer
; r13 = immediate load for conditions and operations 
; r14 = conditional jump checks
render_buffer:
    ; Pseudo 
    ; r1 = 0
    ; r3 = VRAM_START
    ; for r1 < 79
    ;   r2 = 0;
    ;   r5 = RAM[r1] + 0 (GETB NOT GET) (fetch byte in buffer)
    ;   for r2 < 8
    ;       r6 = r5 & 0x01
    ;       RAM[r3] = r6;  (works for now, will change with actual # sprite)
    ;       r3++;

    SET r1 0x0000; loop counter
    SET r3 0xF9DA; VRAM pointer
    SET r12 @buff1; buff1 pointer

    ; loop over every byte inside buff2
    RB_byte_loop:
        ; bit offset counter = 0
        SET r2 0;

        ; OUT r1;

        ; get byte at r1 + buff pointer
        ; add offset
        ADD r1 r1 r12;
        ; get byte
        GETB r5 r1 0;
        ; sub offset (so for loop logic is fine)
        SUB r1 r1 r12;

        RB_bit_loop:

            ; get last bit in byte
            SET r13 0x0001; 
            ; r6 = byte & 0x0001
            AND r6 r5 r13; 

            ; bit shift to right
            SET r13 1;
            RSH r5 r5 r13;

            ; OUT r6;

            ; send to vram!
            ; if (r6 == 1) {draw "#"} else {draw " "}
            IJIF r6 $RB_draw_0;
            RB_draw_1:
                SET r13 35; "#"
                IJMP $RB_draw_end

            RB_draw_0:
                SET r13 32; " "
                IJMP $RB_draw_end

            RB_draw_end:
                STRB r13 r3 0;

            ; increment vram pointer
            IADD r3 r3 1;

            ; end of bit_loop logic (r2 < 8; r2++)
            ; r2++;
            IADD r2 r2 1;
            SET r13 8;
            LT r14 r2 r13; r14 = (r2 < 8)
            IJIT r14 $RB_bit_loop; if ((r2 < 8) == true) {repeat}

        ; end of byte_loop logic (r1 < 79; r1++)
        ; r1++;
        IADD r1 r1 1;
        ; if (r1 > 79) {return}
        SET r13 79;
        LT r14 r13 r1;
        IJIT r14 $RB_return;

        ; jump back to loop
        IJMP $RB_byte_loop;

    RB_return: 
        RET;


; Parameters
; r1 = buff pointer
; Variables
; r2 = loop counter
clear_buffer:
    ; clears buffer being pointed at by r1
    ; once i > buff_size return

    SET r2 0x0000; loop counter
    SET r3 0x0000; 0

    CB_loop: 

        ADD r2 r2 r1;
        STRB r3 r2 0;
        SUB r2 r2 r1;

        ; end of CB_loop logic 
        ; r2++;
        IADD r2 r2 1;

        ; if (r2 > 79) {return}
        SET r13 79;
        GT r14 r2 r13;
        IJIT r14 $CB_return;

        IJMP $CB_loop;

    CB_return:
        RET;

    ; jump back to loop
    IJMP $RB_byte_loop;

; Parameters 
; r1  = buffer src pointer (w+r)
; r2  = buffer dst pointer (w+r)
; Variables
; r3  = loop counter
; r4  = byte buffer
; r14 = immediate loads and conditionals
copy_buffer: 
    ; Pseudo code
    ; for (let i = 0; i < 79; i++) {
    ;      RAM[dst_ptr+i] = RAM[src_ptr+i];
    ; }


    ; for loop: init
    ; i = 0
    SET r3 0x0000; loop counter
    SET r4 0x0000; byte buffer
    CpyBuff_loop:

        ; get byte from src buff
        ADD r3 r3 r1;
        GETB r4 r3 0;
        SUB r3 r3 r1;

        ; str byte to dst buff
        ADD r3 r3 r2;
        ; OUT r3;
        STRB r4 r3 0;
        SUB r3 r3 r2;


        ; for loop: condition
        ; if (i < 79)
        SET r14 79;
        LT r14 r3 r14; (r14 = r3 < r14)
        IJIF r14 $CpyBuff_end;

        ; for loop: increment 
        ; i++
        IADD r3 r3 1;

        IJMP $CpyBuff_loop;

    CpyBuff_end:
        RET;




.data
    ; Need 2 79 byte buffers
    buff1: 0x00 * 79
    buff2: 0xFF * 79