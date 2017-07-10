#include <stdio.h>

#include "prepare_system.h"
#include "matmul.h"

int main(int argc, char* argv[])
{
    prepare_system("1", "23", "1");

    init_data(matmul2d_kj_vec_j_type);
    test_run(matmul2d_kj_vec_j_type);
    check_results(matmul2d_kj_vec_j_type);

    /*    
    init_data(matmul2d_kj_vec_k_type);
    test_run(matmul2d_kj_vec_k_type);
    check_results(matmul2d_kj_vec_k_type);

    init_data(matmul2d_kj_vec_k_no_gather_type);
    test_run(matmul2d_kj_vec_k_no_gather_type);
    check_results(matmul2d_kj_vec_k_no_gather_type);

    init_data(matmul2d_kj_vec_kj_1_type);
    test_run(matmul2d_kj_vec_kj_1_type);
    check_results(matmul2d_kj_vec_kj_1_type);

    init_data(matmul2d_kj_vec_kj_1_no_gather_type);
    test_run(matmul2d_kj_vec_kj_1_no_gather_type);
    check_results(matmul2d_kj_vec_kj_1_no_gather_type);

    init_data(matmul2d_kj_vec_kj_2_type);
    test_run(matmul2d_kj_vec_kj_2_type);
    check_results(matmul2d_kj_vec_kj_2_type);

    init_data(matmul2d_kj_novec_type);
    test_run(matmul2d_kj_novec_type);
    check_results(matmul2d_kj_novec_type);

    init_data(matmul2d_jk_vec_j_type);
    test_run(matmul2d_jk_vec_j_type);
    check_results(matmul2d_jk_vec_j_type);
        
    init_data(matmul2d_jk_vec_k_no_gather_type);
    test_run(matmul2d_jk_vec_k_no_gather_type);
    check_results(matmul2d_jk_vec_k_no_gather_type);

    init_data(matmul2d_jk_vec_k_gather_type);
    test_run(matmul2d_jk_vec_k_gather_type);
    check_results(matmul2d_jk_vec_k_gather_type);
    */
        
    init_data(matmul2d_jk_vec_jk_bcast_type);
    test_run(matmul2d_jk_vec_jk_bcast_type);
    check_results(matmul2d_jk_vec_jk_bcast_type);

    init_data(matmul2d_jk_novec_type);
    test_run(matmul2d_jk_novec_type);
    check_results(matmul2d_jk_novec_type);
    
    return 0;
}
