#include <stddef.h>

void add_box_init(void);

void* add_box_register_cb(void*, size_t);

int add(int, int);

typedef int (*CallbackFn)(void);
void display(CallbackFn fn);

int fn42(void) {
    return 42;
}

int main() {
    add_box_init();

    void* s_fn42 = add_box_register_cb(&fn42, 0);

    display(s_fn42);

    for (size_t i = 0; i < 1000000000; i++) {
        add(10, 32);
    }
    return 0;
}
