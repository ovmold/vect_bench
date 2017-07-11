#include "matmul.h"

/*
    matmul2d_kj_vec_kj_2(A,B,C)
    A -> rdi
    B -> rsi
    C -> rdx
*/
    .text
    .globl    matmul2d_kj_vec_kj_2
    .type    matmul2d_kj_vec_kj_2, @function
matmul2d_kj_vec_kj_2:
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
    /*IACA_START_MARKER*/
    vbroadcastsd   (%rdi), %ymm2                   /* B[i][k + 0] */
    vbroadcastsd  8(%rdi), %ymm3                   /* B[i][k + 1] */
    vbroadcastsd 16(%rdi), %ymm4                   /* B[i][k + 2] */
    vbroadcastsd 24(%rdi), %ymm5                   /* B[i][k + 3] */
    xorl    %eax, %eax                             /* j = 0 */

.L5: /* j loop */
    vmovupd    (%rcx,%rax), %ymm1                     /* A[i][j + 0:3] */

    vmulpd    (%rsi,%rax), %ymm2, %ymm0              /* B[i][k + 0] * C[k + 0][j + 0:3] */
    vaddpd    %ymm1, %ymm0, %ymm1                    /* A[i][j + 0:3] + C[k + 0][j + 0:3] * B[i][k + 0] */

    vmulpd    STR_SIZE(%rsi,%rax), %ymm3, %ymm0      /* B[i][k + 1] * C[k + 1][j + 0:3] */
    vaddpd    %ymm1, %ymm0, %ymm1                    /* A[i][j + 0:3] + C[k + 1][j + 0:3] * B[i][k + 1] */

    vmulpd    STR_SIZE*2(%rsi,%rax), %ymm4, %ymm0    /* B[i][k + 2] * C[k + 2][j + 0:3] */
    vaddpd    %ymm1, %ymm0, %ymm1                    /* A[i][j + 0:3] + C[k + 2][j + 0:3] * B[i][k + 2] */

    vmulpd    STR_SIZE*3(%rsi,%rax), %ymm5, %ymm0    /* B[i][k + 3] * C[k + 3][j + 0:3] */
    vaddpd    %ymm1, %ymm0, %ymm1                    /* A[i][j + 0:3] + C[k + 3][j + 0:3] * B[i][k + 3] */

    vmovupd    %ymm1, (%rcx,%rax)                     /* A[i][j + 0:3] = A[i][j + 0:3] + C[k + 0:3][j + 0:1] * B[i][k + 0:3] */
    
    addq    $32, %rax                              /* j = j + 4 */
    cmpq    $STR_SIZE, %rax                        /* j == STR_SIZE ? j loop is over */
    jne    .L5

    addq    $STR_SIZE*4, %rsi                      /* C[k + 4][j] */   
    addq    $32, %rdi                              /* B[i][k + 4] */
    cmpq    %rdx, %rsi                             /* C[k + 4][j] == C[N - 1][j] ? k loop is over */
    jne    .L10

    /*IACA_END_MARKER*/
.L6:
    addq    $STR_SIZE, %rcx                        /* A[i + 1][j] */
    addq    $STR_SIZE, %r10                        /* B[i + 1][k] */
    cmpq    %r8, %rcx                              /* A[i + 1][j] == A[N - 1][j] ? i loop is over */
    jne    .L2

    vzeroupper

    get_tsc tsc_val_e
        
    ret
