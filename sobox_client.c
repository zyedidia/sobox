#include <stdio.h>
#include <dlfcn.h>

#include "sobox.h"

int
main()
{
    char* host = "127.0.0.1";

    CLIENT* clnt = clnt_create(host, SOBOX, SOBOX_VERSION, "udp");
    if (clnt == NULL) {
        clnt_pcreateerror(host);
        exit(1);
    }

    uint64_t* handle = rdlopen_1(&(struct dlopen_args) {
        .filename = "libfoo/libfoo.so",
        .flags = RTLD_LAZY,
    }, clnt);
    printf("libfoo: %lx\n", *handle);

    uint64_t* sym = rdlsym_1(&(struct dlsym_args) {
        .handle = *handle,
        .symbol = "foo",
    }, clnt);
    printf("symbol: %lx\n", *sym);

    rdlcall_1(&(struct dlcall_args) {
        .symbol = *sym,
    }, clnt);
}
