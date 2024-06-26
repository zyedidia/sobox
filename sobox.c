#include "sobox.h"
#include <stdio.h>
#include <stdlib.h>
#include <rpc/pmap_clnt.h>
#include <string.h>
#include <memory.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <assert.h>

void sobox_1(struct svc_req*, register SVCXPRT*);

void
sobox_server(int notifyfd)
{
    register SVCXPRT *transp;

    pmap_unset (SOBOX, SOBOX_VERSION);

    transp = svcudp_create(RPC_ANYSOCK);
    if (transp == NULL) {
        fprintf (stderr, "%s", "cannot create udp service.");
        exit(1);
    }
    if (!svc_register(transp, SOBOX, SOBOX_VERSION, sobox_1, IPPROTO_UDP)) {
        fprintf (stderr, "%s", "unable to register (SOBOX, SOBOX_VERSION, udp).");
        exit(1);
    }

    transp = svctcp_create(RPC_ANYSOCK, 0, 0);
    if (transp == NULL) {
        fprintf (stderr, "%s", "cannot create tcp service.");
        exit(1);
    }
    if (!svc_register(transp, SOBOX, SOBOX_VERSION, sobox_1, IPPROTO_TCP)) {
        fprintf (stderr, "%s", "unable to register (SOBOX, SOBOX_VERSION, tcp).");
        exit(1);
    }

    close(notifyfd);

    svc_run ();
    fprintf (stderr, "%s", "svc_run returned");
    exit (1);
    /* NOTREACHED */
}

static char* host = "127.0.0.1";
static CLIENT* clnt;

void
sbxdlinit()
{
    int r;
    int pipefd[2];
    r = pipe(pipefd);
    assert(r == 0);
    int pid = fork();
    assert(pid >= 0);
    if (pid == 0) {
        close(pipefd[0]);
        sobox_server(pipefd[1]);
        exit(0);
    }
    close(pipefd[1]);
    char c;
    r = read(pipefd[0], &c, 1);
    assert(r == 0); // pipe closed as notification
    close(pipefd[0]);

    clnt = clnt_create(host, SOBOX, SOBOX_VERSION, "udp");
    if (clnt == NULL) {
        clnt_pcreateerror(host);
        exit(1);
    }
}

uint64_t
sbxdlopen(char* filename, int flags)
{
    uint64_t* handle = sbxdlopen_1(&(struct dlopen_args) {
        .filename = filename,
        .flags = flags,
    }, clnt);
    return *handle;
}

uint64_t
sbxdlsym(uint64_t handle, char* symbol)
{
    uint64_t* sym = sbxdlsym_1(&(struct dlsym_args) {
        .handle = (uint64_t) handle,
        .symbol = symbol,
    }, clnt);
    return *sym;
}

uint64_t
sbxdlcall(uint64_t symbol, uint64_t a0, uint64_t a1, uint64_t a2, uint64_t a3,
        uint64_t a4, uint64_t a5)
{
    uint64_t* sym = sbxdlcall_1(&(struct dlcall_args) {
        .symbol = symbol,
        .args = { a0, a1, a2, a3, a4, a5 },
    }, clnt);
    return *sym;
}
