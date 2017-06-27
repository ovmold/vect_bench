#include "matmul.h"
    .data
	.p2align 16
indcs:
    .int 1, 2, 3, 4
    .p2align 16
mask:
    .int 1, 1, 1, 1
    
	.text
	.p2align 4,,15
	.globl	matmul2d_jk_vec_k
	.type	matmul2d_jk_vec_k, @function
matmul2d_jk_vec_k:
	movq	%rdi, %r9
	leaq	MTRX_SIZE(%rsi), %r11
	leaq	MTRX_SIZE(%rdx), %r10
	leaq	MTRX_SIZE+STR_SIZE(%rdx), %rdi
	movq	%rsi, %r8
.L2:
	movq	%r10, %rcx
	movq	%r9, %rsi
	.p2align 4,,10
	.p2align 3
.L6:
	movq $0, %rcx
	vmovdqu (indcs), %xmm2
    /*set ymm3 to all ones, since it acts as the mask in vgatherdpd*/
	vmovdqu (mask), %ymm3

	vgatherdpd %ymm3,(%rdx,%xmm2,8),%ymm1
    
	movsd	(%rsi), %xmm1
	leaq	-MTRX_SIZE(%rcx), %rax
	movq	%r8, %rdx
	.p2align 4,,10
	.p2align 3
.L3:
	movsd	(%rdx), %xmm0
	addq	$STR_SIZE, %rax
	addq	$8, %rdx
	mulsd	-STR_SIZE(%rax), %xmm0
	cmpq	%rax, %rcx
	addsd	%xmm0, %xmm1
	movsd	%xmm1, (%rsi)
	jne	.L3
	addq	$8, %rcx
	addq	$8, %rsi
	cmpq	%rdi, %rcx
	jne	.L6
	addq	$STR_SIZE, %r8
	addq	$STR_SIZE, %r9
	cmpq	%r11, %r8
	jne	.L2
	rep ret
