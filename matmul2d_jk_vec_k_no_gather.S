#include "matmul.h"
    .text
    .globl    matmul2d_jk_vec_k_no_gather
    .type    matmul2d_jk_vec_k_no_gather, @function
matmul2d_jk_vec_k_no_gather:
    get_tsc tsc_val_b
        
    movq    %rdi, %r9
    leaq    MTRX_SIZE(%rsi), %r11
    leaq    MTRX_SIZE(%rdx), %r10
    leaq    MTRX_SIZE+STR_SIZE(%rdx), %rdi
    movq    %rsi, %r8
.L2: /* i loop */
    movq    %r10, %rcx
    movq    %r9, %rsi
.L6: /* j loop */
//    vxorpd %ymm1, %ymm1, %ymm1
    vmovsd (%rsi), %xmm1                   /* load A[i][j] */
    leaq    -MTRX_SIZE(%rcx), %rax
    movq    %r8, %rdx
.L3: /* k loop */
    //IACA_START_MARKER
    vmovupd (%rdx), %ymm0                       /* load B[i][k + 0:k + 1] */
    /*vinsertf128 $0x1, 16(%rdx), %ymm0, %ymm0     load B[i][k + 2:k + 4] */
    vmovq (%rax), %xmm2
    vmovq STR_SIZE*1(%rax), %xmm3
    vmovq STR_SIZE*2(%rax), %xmm4
    vmovq STR_SIZE*3(%rax), %xmm5
    vmovlhps %xmm3, %xmm2, %xmm2
    vmovlhps %xmm5, %xmm4, %xmm4
    vinsertf128 $0x1, %xmm4, %ymm2, %ymm2       /* C[k + 0 : k + 3][j] */
    addq    $STR_SIZE*4, %rax                   /* k+=4 or C = C += STR_SIZE*4 (offset by column) */
    addq    $8*4, %rdx                          /* k+=4 or B += B + 8*4 (offset by row)*/
    vmulpd %ymm2, %ymm0, %ymm0
    cmpq    %rax, %rcx
    vaddpd %ymm0, %ymm1, %ymm1
    jne    .L3
    //IACA_END_MARKER
    vhaddpd %ymm1, %ymm1, %ymm1
    vperm2f128 $0x1, %ymm1, %ymm1, %ymm0
    vaddpd %ymm0, %ymm1, %ymm1
    vmovsd    %xmm1, (%rsi)
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
