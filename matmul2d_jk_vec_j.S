#include "matmul.h"

/*
    matmul2d_jk_vec_j(A,B,C)
    A -> rdi
    B -> rsi
    C -> rdx
*/  

	.text
	.p2align 4,,15
	.globl	matmul2d_jk_vec_j
	.type	matmul2d_jk_vec_j, @function
matmul2d_jk_vec_j:
    movq %rdx, %r10
    get_tsc tsc_val_b
    movq %r10, %rdx
        
    movq	%rdi, %r9
	leaq	MTRX_SIZE(%rsi), %r11
	leaq	MTRX_SIZE(%rdx), %r10
	leaq	MTRX_SIZE+STR_SIZE(%rdx), %rdi
	movq	%rsi, %r8
.L2: /* i */
	movq	%r10, %rcx
	movq	%r9, %rsi
	.p2align 4,,10
	.p2align 3
.L6: /* j */
    vmovupd (%rsi), %xmm1
    vinsertf128 $0x1, 16(%rsi), %ymm1, %ymm1
	leaq	-MTRX_SIZE(%rcx), %rax
	movq	%r8, %rdx
	.p2align 4,,10
	.p2align 3
.L3: /* k */
    vbroadcastsd (%rdx), %ymm0
	addq	$STR_SIZE, %rax
	addq	$8, %rdx
    vmovupd -STR_SIZE(%rax), %xmm2
    vinsertf128 $0x1, -STR_SIZE+16(%rax), %ymm2, %ymm2
    vmulpd %ymm2, %ymm0, %ymm0
	cmpq	%rax, %rcx
    vaddpd %ymm0, %ymm1, %ymm1
	jne	.L3
    vmovups %xmm1, (%rsi)
    vextractf128 $0x1, %ymm1, 16(%rsi)
	addq	$32, %rcx
	addq	$32, %rsi
	cmpq	%rdi, %rcx
	jne	.L6
	addq	$STR_SIZE, %r8
	addq	$STR_SIZE, %r9
	cmpq	%r11, %r8
	jne	.L2

    get_tsc tsc_val_e
        
	rep ret
