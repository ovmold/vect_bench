#include "matmul.h"

/*
    matmul2d_kj_vec_k(A,B,C)
    A -> rdi
    B -> rsi
    C -> rdx
*/

vindices: /* Array of indices to access C[k + 0:3][j] */                                        
    .int 0*STR_SIZE, 1*STR_SIZE, 2*STR_SIZE, 3*STR_SIZE  
    .p2align 16

	.text
	.p2align 4,,15
	.globl	matmul2d_kj_vec_k
	.type	matmul2d_kj_vec_k, @function
matmul2d_kj_vec_k:
	movq	%rdi, %rcx
	leaq	MTRX_SIZE(%rdi), %r8
	movq	%rdx, %r9
	movq	%rsi, %r10
	addq	$MTRX_SIZE, %rdx
	vmovdqu (vindices), %xmm4    
	vpcmpeqw %ymm3, %ymm3, %ymm3 				    /* set ymm3 to all ones, it acts as the mask in vgatherdpd */
.L2: /* i loop */
	movq	%r9, %rsi
	movq	%r10, %rdi
	.p2align 4,,10
	.p2align 3
.L10: /* k loop */
	vmovupd	(%rdi), %xmm2                           /* B[i][k + 0:1] */
	vinsertf128	$0x1, 16(%rdi), %ymm2, %ymm2        /* B[i][k + 2:3] */	
	xorl %eax, %eax									/* j = 0 */	
	.p2align 4,,10
	.p2align 3
.L5: /* j loop */
    vbroadcastsd (%rcx,%rax), %ymm1                 /* A[i][j] */	
	
	movq (%rsi,%rax), %r11
	vgatherdpd %ymm3, (%r11, %xmm4), %ymm0     	    /* C[k + 0:3][j] */
	
	vmulpd	%ymm2, %ymm0, %ymm0                   	/* C[k + 0:3][j] * B[i][k + 0:3] */
	vaddpd	%ymm1, %ymm0, %ymm0                   	/* A[i][j] + C[k + 0:3][j] * B[i][k + 0:3] */


	
	addq	$8, %rax                              	/* j = j + 1 */
	cmpq	$STR_SIZE, %rax                       	/* j == STR_SIZE ? j loop is over */
	jne	.L5
	addq 	$32, %rdi							  	/* B[i][k + 4] */
	addq	$4*$STR_SIZE, %rsi                      /* C[k + 4][j] */  
	cmpq	%rdx, %rsi                            	/* C[k + 4][j] == C[N - 1][j] ? k loop is over */
	jne	.L10
.L6:
	addq	$STR_SIZE, %rcx                       	/* A[i + 1][j] */
	addq	$STR_SIZE, %r10                       	/* B[i + 1][k] */
	cmpq	%r8, %rcx                             	/* A[i + 1][j] == A[N - 1][j] ? i loop is over */
	jne	.L2
	vzeroupper
	ret
