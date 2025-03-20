#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

int add(int a, int b) {
    return a + b;
}

int add_stack(int a, int b, int c, int d, int e, int f, int g, int h, int i, int j, int k, int l, int m) {
    return a+b+c+d+e+f+g+h+i+j+k+l+m;
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
