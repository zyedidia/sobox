#include <stdio.h>
#include <assert.h>
#include <dlfcn.h>

typedef int (*addfn)(int, int);

void add_box_init(void);

int main(int argc, char** argv) {
    if (argc <= 1) {
        printf("no library to load\n");
        return 0;
    }

    void* handle = dlopen(argv[1], RTLD_NOW);
    if (!handle) {
        printf("dlerror: %s\n", dlerror());
        return 1;
    }

    void (*add_box_init)(void) = dlsym(handle, "add_box_init");
    add_box_init();
    printf("initialized\n");

    void* sym = dlsym(handle, "add");
    assert(sym);

    addfn add = (addfn) sym;
    printf("result: %d\n", add(10, 32));
    return 0;
}
