.global __lfi_trampoline
__lfi_trampoline:
	pushq %r15
	pushq %r14
	pushq %r13
	pushq %r12
	pushq %rbx
	pushq %rbp
	movq lfi_myproc@gottpoff(%rip), %r11
	movq %fs:(%r11), %r11
	movq %rsp, 0(%r11) # kstackp
	movq 16(%r11), %rsp # stack pointer
	movq 16+14*8(%r11), %r14 # base pointer
	mov __lfi_tplntbl_retfn@GOTPCREL(%rip), %r11
	mov (%r11), %r11
	pushq %r11
	xor %r11d, %r11d
	# TODO: make sure r11 is bundle-aligned and within the sandbox
	jmpq *%r12
	int3
