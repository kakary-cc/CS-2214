	.file	"test.cpp"
	.intel_syntax noprefix
	.text
	.globl	i
	.bss
	.align 4
	.type	i, @object
	.size	i, 4
i:
	.zero	4
	.globl	arr
	.align 32
	.type	arr, @object
	.size	arr, 40
arr:
	.zero	40
	.text
	.globl	main
	.type	main, @function
main:
.LFB0:
	push	ebp
.LCFI0:
	mov	ebp, esp
.LCFI1:
	mov	DWORD PTR i, 0
	jmp	.L2
.L3:
	mov	edx, DWORD PTR i
	mov	eax, DWORD PTR i
	imul	edx, edx, 100
	mov	DWORD PTR arr[0+eax*4], edx
	mov	eax, DWORD PTR i
	add	eax, 1
	mov	DWORD PTR i, eax
.L2:
	mov	eax, DWORD PTR i
	cmp	eax, 9
	jle	.L3
	mov	eax, 0
	pop	ebp
.LCFI2:
	ret
.LFE0:
	.size	main, .-main
	.ident	"GCC: (Ubuntu 11.3.0-1ubuntu1~22.04) 11.3.0"
	.section	.note.GNU-stack,"",@progbits
