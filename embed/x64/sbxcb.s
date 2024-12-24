// This file is not compiled into libsobox, but is used to create the
// cbtrampoline object.

.bundle_align_mode 5
// Trampoline for function with no stack args
.bundle_lock
mov 1f(%rip), %r10
jmpq *2f(%rip)
.p2align 4
1: // target function
	.quad 0
2: // frame-size-specific trampoline function
	.quad 0
.bundle_unlock
