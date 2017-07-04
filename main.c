#include <stdio.h>

#include "matmul.h"

int main(int argc, char* argv[])
{

    
    init_data();
    test_run(matmul2d_kj_vec_j_type);
    check_results();
    
    init_data();
    test_run(matmul2d_kj_vec_k_type);
    check_results();

    init_data();
    test_run(matmul2d_kj_novec_type);
    check_results();
    
    init_data();
    test_run(matmul2d_jk_vec_j_type);
    check_results();
        
    init_data();
    test_run(matmul2d_jk_vec_k_type);
    check_results();
        
    init_data();
    test_run(matmul2d_jk_novec_type);
    check_results();
    
    
    return 0;
}
