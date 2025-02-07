#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

int add(int a, int b) {
    return a + b;
}

_Thread_local int x;

void* fn(void* arg) {
    printf("thread local x: %d\n", x++);
    return NULL;
}

void print() {
    pthread_t t;
    pthread_create(&t, NULL, fn, NULL);
    pthread_join(t, NULL);
}

__attribute__((constructor)) void init() {
    printf("testing libadd's constructor\n");
}

typedef int (*CallbackFn)(void);
void display(CallbackFn fn) {
    printf("callback returned: %d\n", fn());
}
