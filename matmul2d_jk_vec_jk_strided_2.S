#include "matmul.h"

/*
    Matrix C is transposed
 */
    
    .text
    .globl    matmul2d_jk_vec_jk_strided_2
    .type    matmul2d_jk_vec_jk_strided_2, @function
matmul2d_jk_vec_jk_strided_2: 
    get_tsc tsc_val_b
        
    movq    %rdi, %r9
    leaq    MTRX_SIZE(%rsi), %r11
    leaq    STR_SIZE(%rdx), %r10
    leaq    MTRX_SIZE+STR_SIZE(%rdx), %rdi
    movq    %rsi, %r8
.L2: /* i */
    movq    %r10, %rcx
    movq    %r9, %rsi
.L6: /* j */
    vxorpd %ymm3, %ymm3, %ymm3
    vxorpd %ymm4, %ymm4, %ymm4
    vxorpd %ymm5, %ymm5, %ymm5
    vxorpd %ymm6, %ymm6, %ymm6

    /*vmovupd (%rsi), %ymm0*/
    
    leaq    -STR_SIZE(%rcx), %rax
    movq    %r8, %rdx
.L3: /* k */
    vmovupd (%rdx), %ymm1
    vmulpd STR_SIZE*0(%rax), %ymm1, %ymm2
    vaddpd %ymm2, %ymm3, %ymm3

    vmulpd STR_SIZE*1(%rax), %ymm1, %ymm2
    vaddpd %ymm2, %ymm4, %ymm4

    vmulpd STR_SIZE*2(%rax), %ymm1, %ymm2
    vaddpd %ymm2, %ymm5, %ymm5

    vmulpd STR_SIZE*3(%rax), %ymm1, %ymm2
    vaddpd %ymm2, %ymm6, %ymm6
    /* maybe do the vmul after incrementation rax and rdx */

    addq    $32, %rax
    addq    $32, %rdx
    cmpq    %rax, %rcx
    jne    .L3

    vhaddpd %ymm3, %ymm3, %ymm3
    vperm2f128 $0x1, %ymm3, %ymm3, %ymm1
    vaddpd %ymm1, %ymm3, %ymm3

    vhaddpd %ymm4, %ymm4, %ymm4
    vperm2f128 $0x1, %ymm4, %ymm4, %ymm1
    vaddpd %ymm1, %ymm4, %ymm4

    vhaddpd %ymm5, %ymm5, %ymm5
    vperm2f128 $0x1, %ymm5, %ymm5, %ymm1
    vaddpd %ymm1, %ymm5, %ymm5

    vhaddpd %ymm6, %ymm6, %ymm6
    vperm2f128 $0x1, %ymm6, %ymm6, %ymm1
    vaddpd %ymm1, %ymm6, %ymm6

    vmovlhps %xmm4, %xmm3, %xmm3
    vmovlhps %xmm6, %xmm5, %xmm5
    vinsertf128 $0x1, %xmm5, %ymm3, %ymm3

    vaddpd (%rsi), %ymm3, %ymm3

    vmovupd %ymm3, (%rsi)
    
    addq    $STR_SIZE*4, %rcx
    addq    $32, %rsi
    cmpq    %rdi, %rcx
    jne    .L6

    addq    $STR_SIZE, %r8
    addq    $STR_SIZE, %r9
    cmpq    %r11, %r8
    jne    .L2

    get_tsc tsc_val_e
        
    rep ret
