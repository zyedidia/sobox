	.file	"add.c"
	.text
	.p2align 4
	.globl	add
	.type	add, @function
add:
.LFB23:
	.cfi_startproc
	endbr64
	movq	x@gottpoff(%rip), %rax
	movl	%edi, %fs:(%rax)
	leal	(%rdi,%rsi), %eax
	ret
	.cfi_endproc
.LFE23:
	.size	add, .-add
	.section	.rodata.str1.1,"aMS",@progbits,1
.LC0:
	.string	"testing libadd's constructor"
	.section	.text.startup,"ax",@progbits
	.p2align 4
	.globl	init
	.type	init, @function
init:
.LFB24:
	.cfi_startproc
	endbr64
	leaq	.LC0(%rip), %rdi
	jmp	puts@PLT
	.cfi_endproc
.LFE24:
	.size	init, .-init
	.section	.init_array,"aw"
	.align 8
	.quad	init
	.section	.rodata.str1.1
.LC1:
	.string	"callback returned: %d\n"
	.text
	.p2align 4
	.globl	display
	.type	display, @function
display:
.LFB25:
	.cfi_startproc
	endbr64
	subq	$8, %rsp
	.cfi_def_cfa_offset 16
	call	*%rdi
	leaq	.LC1(%rip), %rsi
	movl	$1, %edi
	addq	$8, %rsp
	.cfi_def_cfa_offset 8
	movl	%eax, %edx
	xorl	%eax, %eax
	jmp	__printf_chk@PLT
	.cfi_endproc
.LFE25:
	.size	display, .-display
	.globl	x
	.section	.tbss,"awT",@nobits
	.align 4
	.type	x, @object
	.size	x, 4
x:
	.zero	4
	.ident	"GCC: (Ubuntu 11.4.0-1ubuntu1~22.04) 11.4.0"
	.section	.note.GNU-stack,"",@progbits
	.section	.note.gnu.property,"a"
	.align 8
	.long	1f - 0f
	.long	4f - 1f
	.long	5
0:
	.string	"GNU"
1:
	.align 8
	.long	0xc0000002
	.long	3f - 2f
2:
	.long	0x3
3:
	.align 8
4:
