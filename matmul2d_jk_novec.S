#include "matmul.h"

/*
    matmul2d_jk_novec(A,B,C)
    A -> rdi
    B -> rsi
    C -> rdx
*/  
    
    .text
    .p2align 4,,15
    .globl    matmul2d_jk_novec
    .type    matmul2d_jk_novec, @function
matmul2d_jk_novec:
    get_tsc tsc_val_b
        
    movq    %rdi, %r9
    leaq    MTRX_SIZE(%rsi), %r11
    leaq    MTRX_SIZE(%rdx), %r10
    leaq    MTRX_SIZE+STR_SIZE(%rdx), %rdi
    movq    %rsi, %r8
.L2: /* i loop */
    movq    %r10, %rcx
    movq    %r9, %rsi
    .p2align 4,,10
    .p2align 3
.L6: /* j loop */
    movsd    (%rsi), %xmm1
    leaq    -MTRX_SIZE(%rcx), %rax
    movq    %r8, %rdx
    .p2align 4,,10
    .p2align 3
.L3: /* k loop */
    movsd    (%rdx), %xmm0
    addq    $STR_SIZE, %rax
    addq    $8, %rdx
    mulsd    -STR_SIZE(%rax), %xmm0
    cmpq    %rax, %rcx
    addsd    %xmm0, %xmm1
    movsd    %xmm1, (%rsi)
    jne    .L3
    addq    $8, %rcx
    addq    $8, %rsi
    cmpq    %rdi, %rcx
    jne    .L6
    addq    $STR_SIZE, %r8
    addq    $STR_SIZE, %r9
    cmpq    %r11, %r8
    jne    .L2

    get_tsc tsc_val_e
    
    rep ret
