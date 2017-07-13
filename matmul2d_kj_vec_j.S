#include "matmul.h"

/*
    matmul2d_kj_vec_j(A,B,C)
    A -> rdi
    B -> rsi
    C -> rdx
*/
    .text
    .globl    matmul2d_kj_vec_j
    .type    matmul2d_kj_vec_j, @function
matmul2d_kj_vec_j:
    get_tsc tsc_val_b

    movq    %rdi, %rcx
    leaq    MTRX_SIZE(%rdi), %r8
    movq    %rdx, %r9
    movq    %rsi, %r10
    addq    $MTRX_SIZE, %rdx
.L2: /* i loop */
    movq    %r9, %rsi
    movq    %r10, %rdi
.L10: /* k loop */
    vbroadcastsd    (%rdi), %ymm2                 /* B[i][k] */
    xorl    %eax, %eax
.L5: /* j loop */
    IACA_START_MARKER
    vmovupd    (%rsi,%rax), %ymm0                       /* C[k][j + 0:3] */
    vmovupd    (%rcx,%rax), %ymm1                      /* A[i][j + 0:3] */

    vmulpd    %ymm2, %ymm0, %ymm0                   /* C[k][j + 0:3] * B[i][k] */
    vaddpd    %ymm1, %ymm0, %ymm0                   /* A[i][j + 0:3] + C[k][j + 0:3] * B[i][k] */

    vmovupd    %ymm0, (%rcx,%rax)                       /* A[i][j + 0:3] = A[i][j + 0:3] + C[k][j + 0:3] * B[i][k] */
    
    addq    $32, %rax                             /* j = j + 4 */
    cmpq    $STR_SIZE, %rax                       /* j == STR_SIZE ? j loop is over */
    jne    .L5
    IACA_END_MARKER
    addq    $STR_SIZE, %rsi                       /* C[k + 1][j] */  
    addq    $8, %rdi                              /* B[i][k + 1] */
    cmpq    %rdx, %rsi                            /* C[k + 1][j] == C[N - 1][j] ? k loop is over */
    jne    .L10
.L6:
    addq    $STR_SIZE, %rcx                       /* A[i + 1][j] */
    addq    $STR_SIZE, %r10                       /* B[i + 1][k] */
    cmpq    %r8, %rcx                             /* A[i + 1][j] == A[N - 1][j] ? i loop is over */
    jne    .L2
    vzeroupper

    get_tsc tsc_val_e
        
    ret
