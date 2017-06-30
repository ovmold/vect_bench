#ifndef __ASSEMBLER__

#define DEF_TEST(name, fnc, type) type,
enum matmul2d_type {
#include "tests.def"
};
#undef DEF_TEST

void init_data();
void test_run(int);
int check_results();

#else /* __ASSEMBLER__ is defined */

#ifndef N
#error "N is not defined."
#endif
    
#define STR_SIZE N*8
#define MTRX_SIZE N*N*8

.global tsc_val_b
.global tsc_val_e
.global tsc_tmp

.macro get_tsc var
    rdtsc
    mov %eax, \var
    mov %edx, \var+0x4
.endm
#endif /* __ASSEMBLER__ */
