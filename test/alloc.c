#include <stdio.h>

void* sbx_malloc(size_t);

int main() {
    printf("allocate inside sandbox: %p\n", sbx_malloc(10));
    printf("allocate inside sandbox: %p\n", sbx_malloc(10));
    printf("allocate inside sandbox: %p\n", sbx_malloc(10));
    return 0;
}
