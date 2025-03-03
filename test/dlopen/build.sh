#!/bin/sh

set -ex

x86_64-lfi-linux-musl-clang lib.c -shared -fPIC -O2 -o libadd.so
lfi-bind -V -map /lib/libadd.so=$PWD/libadd.so -map /lib/ld-musl-x86_64.so.1=/opt/x86_64-lfi-clang/sysroot/lib/ld-musl-x86_64.so.1 libadd.so
gcc main.c -O2 -llfi -Wl,--export-dynamic-symbol-list=symbols.list -u lfi_myctx -u lfi_new_plat -u lfi_tux_new -u lfi_tux_proc_new -u lfi_host_fdopen -u lfi_tux_ctx -u lfi_ctx_as -u lfi_as_mapany -u lfi_as_mapat -u lfi_tux_soboxinit -u lfi_tux_proc_run -u lfi_strerror -u lfi_thread_init
