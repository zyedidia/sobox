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
