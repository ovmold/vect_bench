#include "matmul.h"

/*
    matmul2d_kj_novec(A,B,C)
    A -> rdi
    B -> rsi
    C -> rdx
*/
    
	.text
	.p2align 4,,15
	.globl	matmul2d_kj_novec
	.type	matmul2d_kj_novec, @function
matmul2d_kj_novec:
	subq	%rsi, %rdi
	leaq	STR_SIZE(%rsi), %r8
	leaq	MTRX_SIZE+STR_SIZE(%rsi), %r10
	leaq	-STR_SIZE(%rdi), %r9
.L2: /* i loop */
	leaq	(%r9,%r8), %rcx
	leaq	-STR_SIZE(%r8), %rsi
	movq	%rdx, %rdi
	.p2align 4,,10
	.p2align 3
.L6: /* k loop */
	xorl	%eax, %eax
	.p2align 4,,10
	.p2align 3
.L3: /* j loop */
	movsd	(%rsi), %xmm0
	mulsd	(%rdi,%rax), %xmm0
	addsd	(%rcx,%rax), %xmm0
	movsd	%xmm0, (%rcx,%rax)
	addq	$8, %rax
	cmpq	$STR_SIZE, %rax
	jne	.L3
	addq	$8, %rsi
	addq	$STR_SIZE, %rdi
	cmpq	%rsi, %r8
	jne	.L6
	addq	$STR_SIZE, %r8
	cmpq	%r8, %r10
	jne	.L2
	rep ret
