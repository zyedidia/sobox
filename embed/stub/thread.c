#include <stdlib.h>
#include <pthread.h>

void* _lfi_thread_create(void* fn) {
    pthread_t* t = malloc(sizeof(pthread_t));
    pthread_create(t, NULL, fn, NULL);
    return t;
}

void _lfi_thread_destroy(void* arg) {
    free((pthread_t*) arg);
}
