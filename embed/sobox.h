#pragma once

#include <stddef.h>

// sbx_malloc allocates a heap object of size 'n' inside the sandbox.
//
// TODO: make sure sbx_malloc returns a pointer inside the sandbox.
//
// TODO: currently sbx_malloc is implemented by calling the malloc symbol
// inside the sandbox. It could also be implemented by keeping an external
// allocator that uses lfi_mmap to map pages inside the sandbox.
void* sbx_malloc(size_t n);

// sbx_free frees a pointer allocated inside the sandbox by sbx_malloc.
void sbx_free(void* p);

// sbx_stackalloc pushes 'n' bytes of space onto the sandbox stack. A pointer
// to the allocated stack space is returned.
//
// TODO: sbx_stackpush is unimplemented
void* sbx_stackpush(size_t n);

// sbx_stackpop pops 'n' bytes of space from the sandbox stack.
//
// TODO: sbx_stackpop is unimplemented
void sbx_stackpop(size_t n);

// sbx_register_cb registers 'fn' as a callback.
//
// Returns a pointer in the sandbox that the sandbox can use to invoke the
// callback. The 'stackframe' argument gives the size of stack arguments that
// must be copied when 'fn' is invoked.
void* sbx_register_cb(void* fn, size_t stackframe);

// sbx_unregister_cb unregisters 'fn' as a callback.
void sbx_unregister_cb(void* fn);

// sbx_addr returns the address in the sandbox of the given sandbox function.
void* sbx_addr(void* fn);
