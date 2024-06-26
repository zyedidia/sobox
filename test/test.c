#include <dlfcn.h>
#include <stdio.h>
#include <stdint.h>

void sbxdlinit();
uint64_t sbxdlopen(char* filename, int flags);
uint64_t sbxdlsym(uint64_t handle, char* symbol);
uint64_t sbxdlcall(uint64_t handle, uint64_t a0, uint64_t a1, uint64_t a2, uint64_t a3, uint64_t a4, uint64_t a5);

typedef void (*fn_t)();

int
main()
{
    sbxdlinit();
    uint64_t x = sbxdlopen("libfoo/libfoo.so", RTLD_LAZY);
    printf("loaded: %lx\n", x);
    uint64_t foo = sbxdlsym(x, "foo");
    printf("symbol foo: %lx\n", foo);
    sbxdlcall(foo, 0, 0, 0, 0, 0, 0);

    return 0;
}
