#define PAGESIZE 16384

1:
ldr x16, 1b+PAGESIZE
ldr x18, 1b+PAGESIZE+8
br x18
nop
