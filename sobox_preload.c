#define _GNU_SOURCE

#include <stdio.h>
#include <dlfcn.h>

static void* (*real_dlopen)(const char*, int);
static void* (*real_dlsym)(void*, const char*);

static void
init()
{
    real_dlsym = dlvsym(RTLD_NEXT, "dlsym", "GLIBC_2.2.5");
    if (!real_dlsym)
        fprintf(stderr, "dlsym load error: %s\n", dlerror());
    real_dlsym = real_dlsym(RTLD_NEXT, "dlsym");
    if (!real_dlsym)
        fprintf(stderr, "dlsym load error: %s\n", dlerror());
    real_dlopen = real_dlsym(RTLD_NEXT, "dlopen");
    if (!real_dlsym)
        fprintf(stderr, "dlopen load error: %s\n", dlerror());
    printf("hooked dlsym: %lx\n", real_dlsym);
    printf("hooked dlopen: %lx\n", real_dlopen);
}

static char* host = "127.0.0.1";

void*
dlopen(const char* filename, int flags)
{
    if (!real_dlopen)
        init();
    return real_dlopen(filename, flags);
}

void*
dlsym(void* handle, const char* symbol)
{
    if (!real_dlsym)
        init();
    return real_dlsym(handle, symbol);
}

void
dlcall(void* symbol)
{
    printf("hi\n");
}
