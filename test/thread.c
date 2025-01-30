#include <stddef.h>
#include <stdio.h>
#include <pthread.h>

int add(int, int);

static void* start(void* arg) {
    printf("%d\n", add(10, 32));
    return NULL;
}

int main() {
    pthread_t thread;
    pthread_create(&thread, NULL, start, NULL);
    pthread_join(thread, NULL);
    return 0;
}
