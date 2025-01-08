#define REG_BASE  x21
#define REG_SYS   x25

.text

.global sbx_cbtrampoline
sbx_cbtrampoline:
	// Entrypoint for callbacks. Callback target is in x10.
	mrs	x11, tpidr_el0
	add	x11, x11, #:tprel_hi12:lfi_myproc, lsl #12
	add	x11, x11, #:tprel_lo12_nc:lfi_myproc
	ldr	x11, [x11]

	// load stack
	ldr x12, [x11]
	// save stack
	mov x13, sp
	str x13, [x11]
	mov sp, x12

	blr x10

	mrs	x11, tpidr_el0
	add	x11, x11, #:tprel_hi12:lfi_myproc, lsl #12
	add	x11, x11, #:tprel_lo12_nc:lfi_myproc
	ldr	x11, [x11]

	// load stack
	ldr x12, [x11]
	// save stack
	mov x13, sp
	str x13, [x11]
	mov sp, x12

	add x30, REG_BASE, w30, uxtw
	ret
