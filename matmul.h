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

#endif /* __ASSEMBLER__ */
