#include "matmul.h"

/*
    matmul2d_kj_novec(A,B,C)
    A -> rdi
    B -> rsi
    C -> rdx
*/
    
    .text
    .globl    matmul2d_kj_novec
    .type    matmul2d_kj_novec, @function
matmul2d_kj_novec:
    get_tsc tsc_val_b
    
    subq    %rsi, %rdi
    leaq    STR_SIZE(%rsi), %r8
    leaq    MTRX_SIZE+STR_SIZE(%rsi), %r10
    leaq    -STR_SIZE(%rdi), %r9
.L2: /* i loop */
    leaq    (%r9,%r8), %rcx
    leaq    -STR_SIZE(%r8), %rsi                 
    movq    %rdx, %rdi
    .p2align 4,,10
    .p2align 3
.L6: /* k loop */
    xorl    %eax, %eax
.L3: /* j loop */
    movsd    (%rsi), %xmm0
    mulsd    (%rdi,%rax), %xmm0
    addsd    (%rcx,%rax), %xmm0
    movsd    %xmm0, (%rcx,%rax) 
    addq    $8, %rax        /* j = j + 1 */
    cmpq    $STR_SIZE, %rax /* j == STR_SIZE ? j loop is over */
    jne    .L3
    addq    $8, %rsi        /* B[i][k + 1] */
    addq    $STR_SIZE, %rdi /* C[k + 1][j] */
    cmpq    %rsi, %r8       /* B[i][k + 1] == B[i + 1][k] ? k loop is over */
    jne    .L6
    addq    $STR_SIZE, %r8 /* B[i+2][k] */
    cmpq    %r8, %r10      /* B[i+2][k] == B[N - 1][k] + STR_SIZE ?  i loop is over */
    jne    .L2

    get_tsc tsc_val_e
        
    rep ret
