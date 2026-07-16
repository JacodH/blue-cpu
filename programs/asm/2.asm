; 2.asm
; more testing for ISA instructions. 
; going to be testing out the stack. 
; gonna run a few calls
; gonna copy a string to a buffer 
; gonna compare the copy to the original 
; functions: 
;  - str_copy
;  - str_print

; Notes: 
; the data section starts at 0x00d4

.text

; 1. print string
SET r2 @string;
; CALL $str_print;

; 2. copy string
SET r2 @string;
SET r3 @buffer;
CALL $str_copy;

; 2.5 print buffer
; SET r2 @buffer;
; CALL $str_print;

; 3. compare string
SET r2 @string;
SET r3 @buffer;
CALL $str_cmp;

; 4. check if str_cmp returns 0
; if it is correct, print
SET r2 @correct;
IJIT r1 $hlt;
CALL $str_print;
HLT;

hlt: 
    HLT;

; r2 = pointer to string
; r3 = pointer to buffer
str_copy: 
    ; Variables
    ; r4 = editable pointer to string
    ; r5 = editable pointer to buffer

    ; copy buffers
    MOV r2 r4; 
    MOV r3 r5;

    str_copy_loop: 
        ; Variables
        ; r6 = char

        ; get char in string
        GETB r6 r4 0;

        ; copy char to buffer
        STRB r6 r5 0;

        ; see if char is null
        EQ r7 r6 r0;
        IJIT r7 $str_copy_end;

        ; increment pointers to next index
        IADD r4 r4 1;
        IADD r5 r5 1;

        IJMP $str_copy_loop;

    str_copy_end: 
        RET;



; Desc: Compares 2 strings char by char
; returns to r1 
;     0 - strings are identical 
;     1 - strings are not equal 
; returns index of first chars that are not equal to r2
;     1 - at pointer + 1 the strings don't match
;     6 - at pointer + 6 the strings don't match 

; Parameters 
; r2 = pointer to string a
; r3 = pointer to string b
str_cmp: 
    ; Variables
    ; r4 = string a pointer
    ; r5 = string b pointer
    ; r8, r9, r10 = comparison registers
    ; r11 = pointer offset (for returning if unequal)

    ; copy pointers
    MOV r2 r4;
    MOV r3 r5;

    SET r11 0;

    str_cmp_loop: 
        ; Variables
        ; r6 = string a char
        ; r7 = string b char 

        ; get string a char
        GETB r6 r4 0;

        ; get string b char
        GETB r7 r5 0;

        ; compare 2 chars
        EQ r8 r6 r7;

        ; if chars are unequal, jump to handler
        IJIF r8 $str_cmp_unequal;

        ; see if string a char is null
        EQ r8 r6 r0;
        ; see if string b char is null
        EQ r9 r7 r0;

        ; check if both end, return equal 
        AND r10 r8 r9;
        IJIT r10 $str_cmp_equal;

        ; check if one only one is null
        XOR r10 r8 r9;
        IJIT r10 $str_cmp_unequal;

        ; increment pointers to next index
        IADD r4 r4 1;
        IADD r5 r5 1;
        IADD r11 r11 1;

        ; compare next chars
        IJMP $str_cmp_loop;

    str_cmp_unequal:
        SET r1 1;
        ; return the index at which the chars were first unequal
        MOV r11 r2;
        RET;

    str_cmp_equal:
        SET r1 0; 
        RET;


; Will OUT every char in a string in RAM given a pointer
; r2 = pointer to string
str_print: 
    ; i want to maintain the fact that r2 is the pointer to the START of the string. ill just copy it to r4

    ; r3 = pointer
    MOV r2 r3; r3 = pointer to string (editable)

    str_print_loop: 
        ; get char
        ; r4 = char = RAM[pointer+0]
        GETB r4 r3 0; 
        
        EQ r5 r0 r4;             r5 = (null == char)
        IJIT r5 $str_print_end;  if r5 is null, return 

        OUT r4;                  output to console

        IADD r3 r3 1;            increment pointer

        IJMP $str_print_loop;    do next char

    str_print_end:
        RET;

.data
    correct: 0x43 0x6f 0x72 0x72 0x65 0x63 0x74 0x21 0x00; "Correct!\0"
    buffer: 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00; 29 byte buffer
    string: 0x54 0x68 0x69 0x73 0x20 0x73 0x74 0x72 0x69 0x6e 0x67 0x20 0x77 0x69 0x6c 0x6c 0x20 0x62 0x65 0x20 0x63 0x6f 0x70 0x69 0x65 0x64 0x2e 0x20 0x00; "This string will be copied\0".