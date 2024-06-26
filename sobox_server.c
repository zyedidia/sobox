#include <dlfcn.h>

#include "sobox.h"

uint64_t*
sbxdlopen_1_svc(dlopen_args* argp, struct svc_req* rqstp)
{
    static uint64_t ret;
    ret = (uint64_t) dlopen(argp->filename, argp->flags);
    printf("calling dlopen(%s, %d) = %lx\n", argp->filename, argp->flags, ret);
    return &ret;
}

uint64_t*
sbxdlsym_1_svc(dlsym_args* argp, struct svc_req* rqstp)
{
    static uint64_t ret;
    ret = (uint64_t) dlsym((void*) argp->handle, argp->symbol);
    printf("calling dlsym(%lx, %s) = %lx\n", argp->handle, argp->symbol, ret);
    return &ret;
}

typedef uint64_t (*dlfn_t)(uint64_t a1, uint64_t a2, uint64_t a3, uint64_t a4, uint64_t a5, uint64_t a6);

uint64_t*
sbxdlcall_1_svc(dlcall_args* argp, struct svc_req* rqstp)
{
    static uint64_t ret;
    dlfn_t fn = (dlfn_t) argp->symbol;
    ret = fn(argp->args[0], argp->args[1], argp->args[2], argp->args[3], argp->args[4], argp->args[5]);
    return &ret;
}
