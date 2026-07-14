SET r0 0;
start:
    IADD r0 r0 1;
    OUT r0;
    IJMP $start;