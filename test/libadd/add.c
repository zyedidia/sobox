#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

int add(int a, int b) {
    return a + b;
}

_Thread_local int x;

void print() {
    printf("thread local x: %d\n", x++);
}

__attribute__((constructor)) void init() {
    printf("testing libadd's constructor\n");
}

typedef int (*CallbackFn)(void);
void display(CallbackFn fn) {
    printf("callback returned: %d\n", fn());
}

void* sbx_thread_create(void* fn) {
    pthread_t* t = malloc(sizeof(pthread_t));
    pthread_create(t, NULL, fn, NULL);
    return t;
}

void sbx_thread_cleanup(void* tv) {
    free(tv);
}
