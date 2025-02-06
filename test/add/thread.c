#include <stddef.h>
#include <stdio.h>
#include <pthread.h>

int add(int, int);
void print(void);

void add_box_init(void);

static void* start(void* arg) {
    printf("%d\n", add(10, 32));
    print();
    print();
    return NULL;
}

int main() {
    add_box_init();

    pthread_t thread;
    pthread_create(&thread, NULL, start, NULL);
    pthread_join(thread, NULL);
    print();
    print();
    return 0;
}
