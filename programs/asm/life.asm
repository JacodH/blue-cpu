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
;   ((25*25) / 8) = 78 bytes.
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
; [ ] move functions for moving buff1 to buff2 and vise versa 
; [ ] grow() die() live() functions
; [ ] testing for all of that
; [ ] looping through all the cells 
; [ ] implementing the rules
; [ ] done? 

.text
main:

    ; copy from buffer 2 to buffer 1
    SET r1 @buff2;
    SET r2 @buff1;
    CALL $copy_buffer;

    ; render buffer 1
    CALL $render_buffer;

    HLT;

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
    ; for r1 < 78
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

        ; end of byte_loop logic (r1 < 78; r1++)
        ; r1++;
        IADD r1 r1 1;
        ; if (r1 > 78) {return}
        SET r13 78;
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

        ; if (r2 > 78) {return}
        SET r13 78;
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
    ; for (let i = 0; i < 78; i++) {
    ;      RAM[dst_ptr+i] = RAM[src_ptr+i];
    ; }


    ; for loop: init
    ; i = 0
    SET r3 0x0000; loop counter
    SET r4 0x0000; byte buffer
    CpyBuff_loop:

        ; get byte from src buff
        ADD r3 r3 r1;
        OUT r3;
        GETB r4 r3 0;
        SUB r3 r3 r1;

        ; str byte to dst buff
        ADD r3 r3 r2;
        ; OUT r3;
        STRB r4 r3 0;
        SUB r3 r3 r2;


        ; for loop: condition
        ; if (i < 78)
        SET r14 78;
        LT r14 r3 r14; (r14 = r3 < r14)
        IJIF r14 $CpyBuff_end;

        ; for loop: increment 
        ; i++
        IADD r3 r3 1;

        IJMP $CpyBuff_loop;

    CpyBuff_end:
        RET;



.data
    ; Need 2 78 byte buffers
    buff1: 0x00 * 78
    buff2: 0xAA * 78