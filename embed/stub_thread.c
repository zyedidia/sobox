#include <pthread.h>
#include <stdlib.h>

void* _lfi_thread_create(void* fn) {
    pthread_t* t = malloc(sizeof(pthread_t));
    pthread_create(t, NULL, fn, NULL);
    return t;
}
