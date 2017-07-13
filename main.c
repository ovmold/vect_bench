#include <stdio.h>

#include "prepare_system.h"
#include "perf_monitor.h"
#include "matmul.h"

int main(int argc, char* argv[])
{
    prepare_system("1", "23", "1");
    PERF_MON_OPEN();

    test_run(matmul2d_kj_vec_j_type);
    test_run(matmul2d_kj_vec_k_type);
    test_run(matmul2d_kj_vec_k_no_gather_type);
    test_run(matmul2d_kj_vec_kj_1_type);
    test_run(matmul2d_kj_vec_kj_1_no_gather_type);
    test_run(matmul2d_kj_vec_kj_2_type);
    test_run(matmul2d_kj_novec_type);
    test_run(matmul2d_jk_vec_j_type);
    test_run(matmul2d_jk_vec_k_no_gather_type);
    test_run(matmul2d_jk_vec_k_gather_type);
    test_run(matmul2d_jk_vec_jk_bcast_type);
    test_run(matmul2d_jk_vec_jk_strided_1_type);
    test_run(matmul2d_jk_vec_jk_strided_2_type);
    test_run(matmul2d_jk_novec_type);
    
    PERF_MON_CLOSE();

    return 0;
}
