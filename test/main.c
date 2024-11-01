#include <stddef.h>

int add(int, int);

int main() {
    for (size_t i = 0; i < 1000000000; i++) {
        add(10, 32);
    }
    return 0;
}
