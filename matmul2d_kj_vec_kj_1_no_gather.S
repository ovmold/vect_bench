#include "matmul.h"

/*
    matmul2d_kj_vec_kj_1_no_gather(A,B,C)
    A -> rdi
    B -> rsi
    C -> rdx
*/
    .text
    .globl    matmul2d_kj_vec_kj_1_no_gather
    .type    matmul2d_kj_vec_kj_1_no_gather, @function
matmul2d_kj_vec_kj_1_no_gather:
    get_tsc tsc_val_b
    
    movq    %rdi, %rcx                                 /* A[0][0] */
    leaq    MTRX_SIZE(%rdi), %r8                    /* A[N - 1][N - 1] */
    leaq    STR_SIZE(%rsi), %r10                     /* B[i + 1][0] */
    push    %rbx
.L2: /* i loop */
    movq    %rdx, %rdi                                 /* C[0][0] */
    leaq    -STR_SIZE(%r10), %rsi                    /* B[i - 1][k] */

.L10: /* k loop */
    vmovupd    (%rsi), %ymm2                           /* B[i][k + 0:3] */
    xorl    %eax, %eax                                /* j = 0 */

.L5: /* j loop */
    xorl    %ebx, %ebx                                /* counter = 0 */

.L7:
    vmovq    (%rdi,%rax), %xmm0                      /* C[k + 0][j] */
    vmovq    STR_SIZE(%rdi,%rax), %xmm1              /* C[k + 1][j] */
    vmovq    STR_SIZE*2(%rdi,%rax), %xmm3            /* C[k + 2][j] */
    vmovq    STR_SIZE*3(%rdi,%rax), %xmm4            /* C[k + 3][j] */  

    vmovlhps    %xmm1, %xmm0, %xmm0
    vmovlhps    %xmm4, %xmm3, %xmm3

    vinsertf128    $0x1, %xmm3, %ymm0, %ymm0           /* C[k + 0:3][j] */

    vmulpd    %ymm2, %ymm0, %ymm0                       /* B[i][k + 0:3] * C[k + 0:3][j + counter] */

    /* Reduce for product */                                                            
    vhaddpd    %ymm0, %ymm0, %ymm0                     /* Horizontal sum of product */
    vperm2f128    $0x1, %ymm0, %ymm0, %ymm1
    vaddpd    %ymm0, %ymm1, %ymm0
    vaddsd    (%rcx,%rax), %xmm0, %xmm0
    vmovsd    %xmm0, (%rcx,%rax)                        /* A[i][j + counter] */
    
    addq    $1, %rbx                                /* counter = counter + 1 */    
    addq    $8, %rax
    cmpq    $4, %rbx
    jne .L7

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
    pop   %rbx
    ret
