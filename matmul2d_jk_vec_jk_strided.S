#include "matmul.h"

/*
    Matrix C is transposed
 */
    
	.text
	.globl	matmul2d_jk_vec_jk_strided
	.type	matmul2d_jk_vec_jk_strided, @function
matmul2d_jk_vec_jk_strided: 
    movq %rdx, %r10
    get_tsc tsc_val_b
    movq %r10, %rdx
        
    movq	%rdi, %r9
	leaq	MTRX_SIZE(%rsi), %r11
	leaq	STR_SIZE(%rdx), %r10
	leaq	MTRX_SIZE+STR_SIZE(%rdx), %rdi
	movq	%rsi, %r8
.L2: /* i */
	movq	%r10, %rcx
	movq	%r9, %rsi
.L6: /* j */
    vmovsd (%rsi), %xmm0
//    vmovupd (%rsi), %xmm0
//    vinsertf128 $0x1, 16(%rsi), %ymm0, %ymm0
	leaq	-STR_SIZE(%rcx), %rax
	movq	%r8, %rdx
.L3: /* k */
    vmovupd (%rdx), %xmm1
    vinsertf128 $0x1, 16(%rdx), %ymm1, %ymm1

    vmulpd STR_SIZE*0(%rax), %ymm1, %ymm1
    vaddpd %ymm1, %ymm0, %ymm0
    /* maybe do the vmul after incrementation rax and rdx */

	addq	$32, %rax
	addq	$32, %rdx
	cmpq	%rax, %rcx
	jne	.L3
    vhaddpd %ymm0, %ymm0, %ymm0
    vperm2f128 $0x1, %ymm0, %ymm0, %ymm1
    vaddpd %ymm1, %ymm0, %ymm0
    
    vmovsd %xmm0, (%rsi)
/*    vovups %xmm1, (%rsi)
    vextractf128 $0x1, %ymm1, 16(%rsi)
*/

    addq	$STR_SIZE, %rcx
	addq	$8, %rsi
	cmpq	%rdi, %rcx
	jne	.L6

    addq	$STR_SIZE, %r8
	addq	$STR_SIZE, %r9
	cmpq	%r11, %r8
	jne	.L2

    get_tsc tsc_val_e
        
	rep ret
