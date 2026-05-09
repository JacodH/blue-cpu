SET r0 5;
SET r1 4;

ADD r2 r0 r1;
OUT r2;          Should out 9

SET r0 10;
SET r1 2;

SUB r2 r0 r1;
OUT r2;          Should out 8

SET r0 5;
SET r1 4;

MUL r2 r0 r1;
OUT r2;          Should return 20

SET r0 100;
SET r1 25;

DIV r2 r0 r1;
OUT r2;          Should return 4

SET r0 -5;
SET r1 7;

SMUL r2 r0 r1;
SOUT r2;         Should return -35

SET r0 100;
SET r1 -5;

SDIV r2 r0 r1;
SOUT r2;         Should return -20

SET r0 5;

IADD r2 r0 10;
SOUT r2;         Should return 15

SET r0 30;

ISUB r2 r0 20;
SOUT r2;         Should return 10

HLT;