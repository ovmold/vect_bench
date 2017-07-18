#include "matmul.h"
    
    .text
    .globl    matmul2d_jk_vec_jk_bcast
    .type    matmul2d_jk_vec_jk_bcast, @function
matmul2d_jk_vec_jk_bcast: 
    get_tsc tsc_val_b
        
    movq    %rdi, %r9
    leaq    MTRX_SIZE(%rsi), %r11
    leaq    MTRX_SIZE(%rdx), %r10
    leaq    MTRX_SIZE+STR_SIZE(%rdx), %rdi
    movq    %rsi, %r8
.L2: /* i */
    movq    %r10, %rcx
    movq    %r9, %rsi
.L6: /* j */
    vmovupd (%rsi), %xmm3
    vinsertf128 $0x1, 16(%rsi), %ymm3, %ymm3
    vmovupd 32(%rsi), %xmm4
    vinsertf128 $0x1, 48(%rsi), %ymm4, %ymm4
    leaq    -MTRX_SIZE(%rcx), %rax
    movq    %r8, %rdx
.L3: /* k */
    vbroadcastsd (%rdx), %ymm0
    vmulpd STR_SIZE*0(%rax), %ymm0, %ymm1
    vmulpd STR_SIZE*0+32(%rax), %ymm0, %ymm2
    vaddpd %ymm1, %ymm3, %ymm3
    vaddpd %ymm2, %ymm4, %ymm4
    vbroadcastsd 8(%rdx), %ymm0
    vmulpd STR_SIZE*1(%rax), %ymm0, %ymm1
    vmulpd STR_SIZE*1+32(%rax), %ymm0, %ymm2
    vaddpd %ymm1, %ymm3, %ymm3
    vaddpd %ymm2, %ymm4, %ymm4
    vbroadcastsd 16(%rdx), %ymm0
    vmulpd STR_SIZE*2(%rax), %ymm0, %ymm1
    vmulpd STR_SIZE*2+32(%rax), %ymm0, %ymm2
    vaddpd %ymm1, %ymm3, %ymm3
    vaddpd %ymm2, %ymm4, %ymm4
    vbroadcastsd 24(%rdx), %ymm0
    vmulpd STR_SIZE*3(%rax), %ymm0, %ymm1
    vmulpd STR_SIZE*3+32(%rax), %ymm0, %ymm2
    vaddpd %ymm1, %ymm3, %ymm3
    vaddpd %ymm2, %ymm4, %ymm4
    /* maybe do the vmul after incrementation rax and rdx */

    addq    $STR_SIZE*4, %rax
    addq    $8*4, %rdx
    cmpq    %rax, %rcx
    jne    .L3

    vmovupd %ymm3, (%rsi)
    /*vextractf128 $0x1, %ymm3, 16(%rsi)*/
    vmovupd %ymm4, 32(%rsi)
    /*vextractf128 $0x1, %ymm4, 48(%rsi)*/

    addq    $64, %rcx
    addq    $64, %rsi
    cmpq    %rdi, %rcx
    jne    .L6

    addq    $STR_SIZE, %r8
    addq    $STR_SIZE, %r9
    cmpq    %r11, %r8
    jne    .L2

    get_tsc tsc_val_e
        
    rep ret
