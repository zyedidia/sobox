#include <stddef.h>

#include "sobox.h"

int add(int, int);

typedef int (*CallbackFn)(void);
void display(CallbackFn fn);

int fn42(void) {
    return 42;
}

int main() {
#ifdef SOBOX
    void* s_fn42 = sbx_register_cb(&fn42, 0);
#else
    void* s_fn42 = &fn42;
#endif

    display(s_fn42);

    for (size_t i = 0; i < 1000000000; i++) {
        add(10, 32);
    }
    return 0;
}
