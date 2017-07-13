#ifndef __ASSEMBLER__

#define DEF_TEST(desc, fnc, init, check, type) type,
enum matmul2d_type {
#include "tests.def"
    matmul2d_number_items,
};
#undef DEF_TEST

void test_run(int);

#else /* __ASSEMBLER__ is defined */

#ifndef N
#error "N is not defined."
#endif

#ifndef HW_CACHELINE_SIZE
#warn "Undefined HW_CACHELINE_SIZE will be set to 64 byte."
#define HW_CACHELINE_SIZE 64
#endif

#define STR_SIZE N*8
#define MTRX_SIZE N*N*8

.global tsc_val_b
.global tsc_val_e
.global tsc_tmp

.macro get_tsc var
    movq %rbx, %r8
    movq %rdx, %r10
    xorl %eax, %eax
    cpuid
    rdtsc
    mov %eax, \var
    mov %edx, \var+0x4
    movq %r10, %rdx
    movq %r8, %rbx
.endm

#ifdef USE_IACA
.macro IACA_START_MARKER
    movl $111, %ebx
    .byte 0x64, 0x67, 0x90
.endm
.macro IACA_END_MARKER
    movl $222, %ebx
    .byte 0x64, 0x67, 0x90
.endm
#else
.macro IACA_START_MARKER
.endm
.macro IACA_END_MARKER
.endm
#endif

#endif /* __ASSEMBLER__ */
