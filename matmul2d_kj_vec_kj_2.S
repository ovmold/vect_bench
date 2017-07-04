#include "matmul.h"

/*
    matmul2d_kj_vec_kj_2(A,B,C)
    A -> rdi
    B -> rsi
    C -> rdx
*/
	.text
	.globl	matmul2d_kj_vec_kj_2
	.type	matmul2d_kj_vec_kj_2, @function
matmul2d_kj_vec_kj_2:
    movq %rdx, %r10
    get_tsc tsc_val_b
    movq %r10, %rdx

    movq	%rdi, %rcx
	leaq	MTRX_SIZE(%rdi), %r8
	movq	%rdx, %r9
	movq	%rsi, %r10
	addq	$MTRX_SIZE, %rdx

.L2: /* i loop */
	movq	%r9, %rsi
	movq	%r10, %rdi

.L10: /* k loop */
	vbroadcastsd (%rdi), %ymm2                    /* B[i][k + 0] */
	xorl	%eax, %eax                            /* j = 0 */

.L5: /* k + 0,  j loop */
	vmovupd	(%rsi,%rax), %xmm0                    /* C[k + 0][j + 0:1] */
	vinsertf128	$0x1, 16(%rsi,%rax), %ymm0, %ymm0 /* C[k + 0][j + 2:3] */

	vmovupd	(%rcx,%rax), %xmm1                    /* A[i][j + 0:1] */
	vinsertf128	$0x1, 16(%rcx,%rax), %ymm1, %ymm1 /* A[i][j + 2:3] */

	vmulpd	%ymm2, %ymm0, %ymm0                   /* C[k + 0][j + 0:3] * B[i][k + 0] */

	vaddpd	%ymm1, %ymm0, %ymm0                   /* A[i][j + 0:3] + C[k + 0][j + 0:3] * B[i][k + 0] */
	vmovups	%xmm0, (%rcx,%rax)                    /* A[i][j + 0:1] = A[i][j + 0:1] + C[k + 0][j + 0:1] * B[i][k + 0] */
	vextractf128 $0x1, %ymm0, 16(%rcx,%rax)       /* A[i][j + 2:3] = A[i][j + 2:3] + C[k + 0][j + 2:3] * B[i][k + 0] */
	
    addq	$32, %rax                             /* j = j + 4 */
	cmpq	$STR_SIZE, %rax                       /* j == STR_SIZE ? j loop is over */
	jne	.L5

    addq	$STR_SIZE, %rsi                       /* C[k + 1][j] */  
	addq	$8, %rdi                              /* B[i][k + 1] */

    vbroadcastsd (%rdi), %ymm2                    /* B[i][k + 1] */
    xorl	%eax, %eax

.L7: /* k + 1, j loop */
	vmovupd	(%rsi,%rax), %xmm0                    /* C[k + 1][j + 0:1] */
	vinsertf128	$0x1, 16(%rsi,%rax), %ymm0, %ymm0 /* C[k + 1][j + 2:3] */

	vmovupd	(%rcx,%rax), %xmm1                    /* A[i][j + 0:1] */
	vinsertf128	$0x1, 16(%rcx,%rax), %ymm1, %ymm1 /* A[i][j + 2:3] */

	vmulpd	%ymm2, %ymm0, %ymm0                   /* C[k + 1][j + 0:3] * B[i][k + 1] */

	vaddpd	%ymm1, %ymm0, %ymm0                   /* A[i][j + 0:3] + C[k + 1][j + 0:3] * B[i][k + 1] */
	vmovups	%xmm0, (%rcx,%rax)                    /* A[i][j + 0:1] = A[i][j + 0:1] + C[k + 1][j + 0:1] * B[i][k + 1] */
	vextractf128 $0x1, %ymm0, 16(%rcx,%rax)       /* A[i][j + 2:3] = A[i][j + 2:3] + C[k + 1][j + 2:3] * B[i][k + 1] */
	
    addq	$32, %rax                             /* j = j + 4 */
	cmpq	$STR_SIZE, %rax                       /* j == STR_SIZE ? j loop is over */
	jne	.L7

    addq	$STR_SIZE, %rsi                       /* C[k + 2][j] */  
	addq	$8, %rdi                              /* B[i][k + 2] */
    vbroadcastsd (%rdi), %ymm2                    /* B[i][k + 2] */
    xorl	%eax, %eax

.L8: /* k + 2, j loop */
	vmovupd	(%rsi,%rax), %xmm0                    /* C[k + 2][j + 0:1] */
	vinsertf128	$0x1, 16(%rsi,%rax), %ymm0, %ymm0 /* C[k + 2][j + 2:3] */

	vmovupd	(%rcx,%rax), %xmm1                    /* A[i][j + 0:1] */
	vinsertf128	$0x1, 16(%rcx,%rax), %ymm1, %ymm1 /* A[i][j + 2:3] */

	vmulpd	%ymm2, %ymm0, %ymm0                   /* C[k + 2][j + 0:3] * B[i][k + 2] */

	vaddpd	%ymm1, %ymm0, %ymm0                   /* A[i][j + 0:3] + C[k + 2][j + 0:3] * B[i][k + 2] */
	vmovups	%xmm0, (%rcx,%rax)                    /* A[i][j + 0:1] = A[i][j + 0:1] + C[k + 2][j + 0:1] * B[i][k + 2] */
	vextractf128 $0x1, %ymm0, 16(%rcx,%rax)       /* A[i][j + 2:3] = A[i][j + 2:3] + C[k + 2][j + 2:3] * B[i][k + 2] */
	
    addq	$32, %rax                             /* j = j + 4 */
	cmpq	$STR_SIZE, %rax                       /* j == STR_SIZE ? j loop is over */
	jne	.L8

    addq	$STR_SIZE, %rsi                       /* C[k + 3][j] */  
	addq	$8, %rdi                              /* B[i][k + 3] */
    vbroadcastsd (%rdi), %ymm2                    /* B[i][k + 3] */
    xorl	%eax, %eax

.L9: /* k + 3,  j loop */
	vmovupd	(%rsi,%rax), %xmm0                    /* C[k + 3][j + 0:1] */
	vinsertf128	$0x1, 16(%rsi,%rax), %ymm0, %ymm0 /* C[k + 3][j + 2:3] */

	vmovupd	(%rcx,%rax), %xmm1                    /* A[i][j + 0:1] */
	vinsertf128	$0x1, 16(%rcx,%rax), %ymm1, %ymm1 /* A[i][j + 2:3] */

	vmulpd	%ymm2, %ymm0, %ymm0                   /* C[k + 3][j + 0:3] * B[i][k + 3] */

	vaddpd	%ymm1, %ymm0, %ymm0                   /* A[i][j + 0:3] + C[k][j + 0:3] * B[i][k + 3] */
	vmovups	%xmm0, (%rcx,%rax)                    /* A[i][j + 0:1] = A[i][j + 0:1] + C[k + 3][j + 0:1] * B[i][k + 3] */
	vextractf128 $0x1, %ymm0, 16(%rcx,%rax)       /* A[i][j + 2:3] = A[i][j + 2:3] + C[k + 3][j + 2:3] * B[i][k + 3] */
	
    addq	$32, %rax                             /* j = j + 4 */
	cmpq	$STR_SIZE, %rax                       /* j == STR_SIZE ? j loop is over */
	jne	.L9

	addq	$STR_SIZE, %rsi                       /* C[k + 4][j] */   
	addq	$8, %rdi                              /* B[i][k + 4] */
	cmpq	%rdx, %rsi                            /* C[k + 4][j] == C[N - 1][j] ? k loop is over */
	jne	.L10

.L6:
	addq	$STR_SIZE, %rcx                       /* A[i + 1][j] */
	addq	$STR_SIZE, %r10                       /* B[i + 1][k] */
	cmpq	%r8, %rcx                             /* A[i + 1][j] == A[N - 1][j] ? i loop is over */
	jne	.L2

	vzeroupper

    get_tsc tsc_val_e
        
    ret
