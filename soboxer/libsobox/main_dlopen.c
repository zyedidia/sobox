#include <stdio.h>
#include <assert.h>
#include <dlfcn.h>

typedef int (*addfn)(int, int);

int main(int argc, char** argv) {
    if (argc <= 1) {
        printf("no library to load\n");
        return 0;
    }
    void* handle = dlopen(argv[1], RTLD_LAZY);
    if (!handle) {
        printf("%s\n", argv[1]);
        printf("error: %s\n", dlerror());
    }
    assert(handle);

    void* sym = dlsym(handle, "add");
    assert(sym);

    addfn add = (addfn) sym;
    for (size_t i = 0; i < 1000000000; i++) {
        add(10, 32);
    }
    printf("result: %d\n", add(10, 30));
    return 0;
}
