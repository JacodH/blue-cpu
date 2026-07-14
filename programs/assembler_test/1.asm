.text
SET r1 $string; r0 = pointer to string

print_loop:
    GET r2 r1 0; get char at pointer

    ; r0 = null term

    ; r1 = pointer
    ; r2 = char

    EQ r3 r2 r0; compare char against null (0x00)
    JIF r3 $print_break; if char == null, break

    IADD r1 r1 1; increment the pointer to the next char

    OUT r2; emulator print (for now), could just put this to the screen pointer once that exists. 

    JMP $print_loop; loop back

print_break:
    HLT;

.data
    string: 0x42 0x65 0x6c 0x6c 0x6f 0x21 0x00; "Bello!"