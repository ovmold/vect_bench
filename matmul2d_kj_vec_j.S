#include "matmul.h"

	.text
	.p2align 4,,15
	.globl	matmul2d_kj_vec_j
	.type	matmul2d_kj_vec_j, @function
matmul2d_kj_vec_j:
    movq %rdx, %r10
    get_tsc tsc_val_b
    movq %r10, %rdx

    movq	%rdi, %rcx
	leaq	MTRX_SIZE(%rdi), %r8
	movq	%rdx, %r9
	movq	%rsi, %r10
	addq	$MTRX_SIZE, %rdx
.L2:
	movq	%r9, %rsi
	movq	%r10, %rdi
	.p2align 4,,10
	.p2align 3
.L10:
	vbroadcastsd	(%rdi), %ymm2
	xorl	%eax, %eax
	.p2align 4,,10
	.p2align 3
.L5:
	vmovupd	(%rsi,%rax), %xmm0
	vinsertf128	$0x1, 16(%rsi,%rax), %ymm0, %ymm0
	vmovupd	(%rcx,%rax), %xmm1
	vinsertf128	$0x1, 16(%rcx,%rax), %ymm1, %ymm1
	vmulpd	%ymm2, %ymm0, %ymm0
	vaddpd	%ymm1, %ymm0, %ymm0
	vmovups	%xmm0, (%rcx,%rax)
	vextractf128	$0x1, %ymm0, 16(%rcx,%rax)
	addq	$32, %rax
	cmpq	$STR_SIZE, %rax
	jne	.L5
	addq	$STR_SIZE, %rsi
	addq	$8, %rdi
	cmpq	%rdx, %rsi
	jne	.L10
.L6:
	addq	$STR_SIZE, %rcx
	addq	$STR_SIZE, %r10
	cmpq	%r8, %rcx
	jne	.L2
	vzeroupper

    get_tsc tsc_val_e
        
    ret
