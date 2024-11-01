#include <stdio.h>

int add(int a, int b) {
    return a + b;
}

__attribute__((constructor)) void init() {
    printf("testing libadd's constructor\n");
}
