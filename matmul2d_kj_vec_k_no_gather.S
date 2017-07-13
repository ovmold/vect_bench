#include "matmul.h"

/*
    matmul2d_kj_vec_k_no_gather(A,B,C)
    A -> rdi
    B -> rsi
    C -> rdx
*/

/* vindices:   Array of indices to access C[k + 0:3][j] 
    .int 0*STR_SIZE, 1*STR_SIZE, 2*STR_SIZE, 3*STR_SIZE 
*/ 

    .text
    .globl    matmul2d_kj_vec_k_no_gather
    .type    matmul2d_kj_vec_k_no_gather, @function
matmul2d_kj_vec_k_no_gather:
    get_tsc tsc_val_b
    
    movq    %rdi, %rcx                                 /* A[0][0] */
    leaq    MTRX_SIZE(%rdi), %r8                    /* A[N - 1][N - 1] */
    leaq    STR_SIZE(%rsi), %r10                     /* B[i + 1][0] */
.L2: /* i loop */
    movq    %rdx, %rdi                                 /* C[0][0] */
    leaq    -STR_SIZE(%r10), %rsi                    /* B[i - 1][k] */

.L10: /* k loop */
    vmovupd    (%rsi), %ymm2                           /* B[i][k + 0:1] */
    xorl    %eax, %eax                                /* j = 0 */

.L5: /* j loop */
    vmovq    (%rdi,%rax), %xmm0                      /* C[k + 0][j] */
    vmovq    STR_SIZE(%rdi,%rax), %xmm1              /* C[k + 1][j] */
    vmovq    STR_SIZE*2(%rdi,%rax), %xmm3            /* C[k + 2][j] */
    vmovq    STR_SIZE*3(%rdi,%rax), %xmm4            /* C[k + 3][j] */  

    vmovlhps    %xmm1, %xmm0, %xmm0
    vmovlhps    %xmm4, %xmm3, %xmm3

    vinsertf128    $0x1, %xmm3, %ymm0, %ymm0           /* C[k + 0:3][j] */

    vmulpd    %ymm2, %ymm0, %ymm0                       /* B[i][k + 0:3] * C[k + 0:3][j] */

    /* Reduce for B[i][k + 0:3] * C[k + 0:3][j] */                                                            
    vhaddpd    %ymm0, %ymm0, %ymm0                     /* Horizontal sum of products */
    vperm2f128    $0x1, %ymm0, %ymm0, %ymm1
    vaddpd    %ymm0, %ymm1, %ymm0
    vaddsd    (%rcx,%rax), %xmm0, %xmm0
    vmovsd    %xmm0, (%rcx,%rax)

    addq    $8, %rax                                  /* j = j + 1 */
    cmpq    $STR_SIZE, %rax                           /* j == STR_SIZE ? j loop is over */
    jne    .L5

    addq     $32, %rsi                                  /* B[i][k + 4] */
    addq    $STR_SIZE*4, %rdi                       /* C[k + 4][j] */ 
    cmpq     %rsi, %r10                              /* B[i][k + 4] == B[i][N - 1] ? k loop is over */
    jne    .L10

.L6:
    addq    $STR_SIZE, %rcx                           /* A[i + 1][j] */
    addq    $STR_SIZE, %r10                           /* B[i + 1][0] */
    cmpq    %r8, %rcx                                 /* A[i + 1][j] == A[N - 1][j] ? i loop is over */
    jne    .L2

    vzeroupper

    get_tsc tsc_val_e

    ret
