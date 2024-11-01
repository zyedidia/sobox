# Sobox

A tool for doing library sandboxing with [LFI](https://github.com/zyedidia/lfi).

This tool makes it possible to put shared libraries into lightweight sandboxes.
The overhead of calling functions in a sandboxed library is only 2x more
expensive than a function call to a native library. This is orders of magnitude
less expensive than most hardware-based isolation methods.

Suppose you have an application that currently links against some third-party
library `libfoo.so`. In order to sandbox it you can do the following:

First, recompile `libfoo.so` with the LFI GCC toolchain. This gives you
`libfoo.lfi.so`. However, this shared library is not directly usable from
application code, because it must be loaded and run in an LFI sandbox/runtime
execution environment.

The sobox tool can convert `libfoo.lfi.so` into `libfoo.box.so`, a shared
library that exports the same functions but internally loads and runs
`libfoo.lfi.so` in an LFI sandbox. All exported functions from `libfoo.box.so`
point to trampolines that securely transfer execution into the `libfoo.lfi.so`
running inside the sandbox.

# Usage

The `sobox` tool is built by running `go build`. When you invoke `sobox`, you
provide the shared library to wrap, along with any dependencies that it has,
such as `libc.so`, `libstdc++.so` and the dynamic linker. These libraries are
provided by the LFI GCC toolchain.

```
sobox -map libc.so=/path/to/lfi/libc.so \
    -map ld-musl-x86_64.so.1=/path/to/lfi/ld-musl-x86_64.so.1 \
    -o libfoo.box.so libfoo.lfi.so
```

This will produce a self-contained shared object file that can be linked with
your application.

Warning: this tool is in-progress and subject to change. Currently only x86-64
is supported.

Warning: currently only functions with up to 6 arguments 64-bit integer
arguments are supported (this will be fixed before the tool is considered
complete).

In the future I expect to add options to control the behavior of the runtime
environment (what system calls/files are accessible to sandboxed libraries)

See [notes.md](./notes.md) for some more details about how the sandboxed library
is wrapped.

# Performance

Limited benchmarking has been done on an AMD Ryzen 9 7950X CPU. Below are
measurements for calling an `int add(int, int)` function with and without
sandboxing.

| Platform | Cycles | Instructions |
| --- | --- | --- |
| Native | 9 | 9 |
| Sobox-LFI | 19 | 43 |

# `dlopen`

Usage of Sobox libraries with `dlopen` is not officially supported at the
moment (this could be supported in the future). Currently, libraries still are
likely to work with Glibc's `dlopen` as long as there is enough static TLS
space, since Glibc reserves extra static TLS. Musl does not, and rejects
attempts to `dlopen` shared libraries that use initial-exec TLS.
