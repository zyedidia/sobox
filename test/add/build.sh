#!/bin/sh

set -ex

aarch64-lfi-linux-musl-clang -O2 libadd/add.c -c -o libadd/add.o
ar rcs libadd/libadd.a libadd/add.o
lfibind -V libadd/libadd.a
gcc thread.c libadd_box.a -llfi
