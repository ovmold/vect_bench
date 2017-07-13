#include <stdio.h>
#include <stdlib.h>
#include <sched.h>
#include <malloc.h>
#include <math.h>
#include <omp.h>

#include "matmul.h"

#ifndef N
#error "N is not defined."
#endif

#ifndef EPS
#error "Tolerance EPS is no defined."
#endif

#define DATA_TYPE double
#define Q 16
#define STR_LEN 100

void matmul2d_jk_novec(DATA_TYPE [][N],
                       DATA_TYPE [][N],
                       DATA_TYPE [][N]);

void matmul2d_kj_novec(DATA_TYPE [][N],
                       DATA_TYPE [][N],
                       DATA_TYPE [][N]);

void matmul2d_kj_vec_k(DATA_TYPE [][N],
                       DATA_TYPE [][N],
                       DATA_TYPE [][N]);

void matmul2d_kj_vec_k_no_gather(DATA_TYPE [][N],
                                 DATA_TYPE [][N],
                                 DATA_TYPE [][N]);

void matmul2d_kj_vec_j(DATA_TYPE [][N],
                       DATA_TYPE [][N],
                       DATA_TYPE [][N]);

void matmul2d_kj_vec_kj_1(DATA_TYPE [][N],
                          DATA_TYPE [][N],
                          DATA_TYPE [][N]);

void matmul2d_kj_vec_kj_1_no_gather(DATA_TYPE [][N],
                                    DATA_TYPE [][N],
                                    DATA_TYPE [][N]);

void matmul2d_kj_vec_kj_2(DATA_TYPE [][N],
                          DATA_TYPE [][N],
                          DATA_TYPE [][N]);

void matmul2d_jk_vec_k_gather(DATA_TYPE [][N],
                              DATA_TYPE [][N],
                              DATA_TYPE [][N]);
void matmul2d_jk_vec_k_no_gather(DATA_TYPE [][N],
                                 DATA_TYPE [][N],
                                 DATA_TYPE [][N]);

void matmul2d_jk_vec_j(DATA_TYPE [][N],
                       DATA_TYPE [][N],
                       DATA_TYPE [][N]);

void matmul2d_jk_vec_jk_bcast(DATA_TYPE [][N],
                              DATA_TYPE [][N],
                              DATA_TYPE [][N]);
void matmul2d_jk_vec_jk_strided_1(DATA_TYPE [][N],
                                  DATA_TYPE [][N],
                                  DATA_TYPE [][N]);
void matmul2d_jk_vec_jk_strided_2(DATA_TYPE [][N],
                                  DATA_TYPE [][N],
                                  DATA_TYPE [][N]);

void default_init(DATA_TYPE []);
void transpose_C_init(DATA_TYPE QQ[]);
int default_checker();
int transpose_C_checker();

typedef void (*test_entry_ptr)(DATA_TYPE [][N],
                               DATA_TYPE [][N],
                               DATA_TYPE [][N]);
typedef void (*init_ptr)(DATA_TYPE []);
typedef int (*checker_ptr)();

typedef struct tests_desc_s {
    char desc[STR_LEN];
    test_entry_ptr tst_entry;
    init_ptr initializer;
    checker_ptr checker;
} tests_desc_t;

static tests_desc_t matmul2d_descs[] = {
#define DEF_TEST(desc, fnc, init, check, type) {desc, fnc, init, check},
#include "tests.def"
#undef DEF_TEST
};

static DATA_TYPE correct_instance[N][N];
static DATA_TYPE B[N][N];
static DATA_TYPE C[N][N];
static DATA_TYPE A[N][N];

long unsigned int tsc_val_b;
long unsigned int tsc_val_e;

static int is_double_equal(double a, double b) 
{
    if (fabs(a - b) > EPS)
        return 0;

    return 1;
}

int transpose_C_checker()
{
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            for (int k = 0; k < N; k++) {
                correct_instance[i][j] +=  B[i][k] * C[j][k];
            }
        }
    }

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if (!is_double_equal(A[i][j], correct_instance[i][j])) {
                fprintf(stderr, "A[%d][%d] is not correct\n", i, j);
                fprintf(stderr, "A[%d][%d] (%.20lf) != (%.20lf)\n",
                        i, j, A[i][j], correct_instance[i][j]);
                return 1;
            }
        }
    }

    return 0;
}

int default_checker()
{
    for (int i = 0; i < N; i++) {
        for (int k = 0; k < N;  k++) {
            for (int j = 0; j < N; j++) {
                correct_instance[i][j] +=  B[i][k] * C[k][j];
            }
        }
    }

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if (!is_double_equal(A[i][j], correct_instance[i][j])) {
                fprintf(stderr, "A[%d][%d] is not correct\n", i, j);
                fprintf(stderr, "A[%d][%d] (%.20lf) != (%.20lf)\n",
                        i, j, A[i][j], correct_instance[i][j]);
                return 1;
            }
        }
    }

    return 0;
}

void default_init(DATA_TYPE QQ[])
{
    int k = 0;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            A[i][j] = correct_instance[i][j] = QQ[k++]*rand()/RAND_MAX;
            if (k >= Q) k=0;
            B[i][j]=QQ[k++]*rand()/RAND_MAX;
            if (k >= Q) k=0;
            C[i][j]=QQ[k++]*rand()/RAND_MAX;
            if (k >=Q ) k=0;
        }
    }
}

void transpose_C_init(DATA_TYPE QQ[])
{
    int k = 0;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            A[i][j] = correct_instance[i][j] = 0;
            if (k >= Q) k=0;
            B[i][j]=QQ[k++]*rand()/RAND_MAX;
            if (k >= Q) k=0;
            C[j][i]=QQ[k++]*rand()/RAND_MAX;
            if (k >=Q ) k=0;
        }
    }
}

void init_data(int id)
{
    FILE *fgg=NULL;

    DATA_TYPE QQ[Q];

    if ((fgg=fopen("data","rt"))==NULL) {
        fprintf(stderr,"\nError: Can't find data!\n");
        return ;
    }

    for (int i = 0; i < Q; i++) {
        fscanf(fgg, "%lf", &QQ[i]);
    }

    fclose(fgg);

    printf("matrix dimension: %u\n", N);
    
    matmul2d_descs[id].initializer(QQ);
}

void test_run(int id)
{
    if ((id < matmul2d_jk_novec_type) ||
        (id >= matmul2d_number_items)) {
        fprintf(stderr, "%d is unknown test\n", id);
        return ;
    }

    init_data(id);

    tsc_val_b = tsc_val_e = 0;
    printf("Test <%s> will run\n", matmul2d_descs[id].desc);
    sched_yield();
    double t = omp_get_wtime();
    matmul2d_descs[id].tst_entry(A, B, C);
    t = omp_get_wtime() - t;
    printf("Execution time of matmul2d: %lf [%lu]\n",
           t, tsc_val_e - tsc_val_b);
}

int check_results(int id)
{
    if ((id < matmul2d_jk_novec_type) ||
        (id >= matmul2d_number_items)) {
        fprintf(stderr, "%d is unknown test\n", id);
        return 1;
    }

    if (matmul2d_descs[id].checker())
        return 1;

    printf("Results are correct\n");
    return 0;
}
