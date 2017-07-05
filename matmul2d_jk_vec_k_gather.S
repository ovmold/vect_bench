#include "matmul.h"
    .data
	.p2align 16
vindices:
    .int 0*STR_SIZE, 1*STR_SIZE, 2*STR_SIZE, 3*STR_SIZE
    
	.text
	.p2align 4,,15
	.globl	matmul2d_jk_vec_k_gather
	.type	matmul2d_jk_vec_k_gather, @function
matmul2d_jk_vec_k_gather:
    movq %rdx, %r10
    get_tsc tsc_val_b
    movq %r10, %rdx

	vmovdqu (vindices), %xmm2
    /*    mov STR_SIZE*0, %xmm4
    mov STR_SIZE*1, %ecx
    mov STR_SIZE*2, %ecx
    mov STR_SIZE*3, %ecx*/
        
    movq	%rdi, %r9
	leaq	MTRX_SIZE(%rsi), %r11
	leaq	MTRX_SIZE(%rdx), %r10
	leaq	MTRX_SIZE+STR_SIZE(%rdx), %rdi
	movq	%rsi, %r8
.L2: /* i loop */
	movq	%r10, %rcx
	movq	%r9, %rsi
	.p2align 4,,10
	.p2align 3
.L6: /* j loop */
//    vxorpd %ymm1, %ymm1, %ymm1
    vmovsd (%rsi), %xmm1                   /* load A[i][j] */
	leaq	-MTRX_SIZE(%rcx), %rax
	movq	%r8, %rdx
	.p2align 4,,10
	.p2align 3
.L3: /* k loop */
    vmovupd (%rdx), %xmm0                       /* load B[i][k + 0:k + 1] */
    vinsertf128 $0x1, 16(%rdx), %ymm0, %ymm0    /* load B[i][k + 2:k + 4] */
    vpcmpeqw %ymm3, %ymm3, %ymm3                /* set mask (ymm1) to 1 1 1 1 */
    vgatherdpd %ymm3, (%rax, %xmm2), %ymm4      /* load C[k + 0:k + 4][j] */
	addq	$STR_SIZE*4, %rax                   /* k+=4 or C = C += STR_SIZE*4 (offset by column) */
	addq	$8*4, %rdx                          /* k+=4 or B += B + 8*4 (offset by row)*/
    vmulpd %ymm4, %ymm0, %ymm0
	cmpq	%rax, %rcx
    vaddpd %ymm0, %ymm1, %ymm1
	jne	.L3
    vhaddpd %ymm1, %ymm1, %ymm1
    vperm2f128 $0x1, %ymm1, %ymm1, %ymm0
    vaddpd %ymm0, %ymm1, %ymm1
    vmovsd	%xmm1, (%rsi)
    addq	$8, %rcx
	addq	$8, %rsi
	cmpq	%rdi, %rcx
	jne	.L6
	addq	$STR_SIZE, %r8
	addq	$STR_SIZE, %r9
	cmpq	%r11, %r8
	jne	.L2

    get_tsc tsc_val_e
    
	rep ret
