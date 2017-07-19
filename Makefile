BIN_matmul = matmul
OBJS_matmul = main.o matmul2d.o \
matmul2d_jk_novec.o matmul2d_kj_novec.o \
matmul2d_kj_vec_k.o matmul2d_kj_vec_k_no_gather.o matmul2d_kj_vec_j.o \
matmul2d_kj_vec_kj_1.o matmul2d_kj_vec_kj_1_no_gather.o matmul2d_kj_vec_kj_2.o \
matmul2d_jk_vec_k_gather.o matmul2d_jk_vec_k_no_gather.o matmul2d_jk_vec_j.o \
matmul2d_jk_vec_jk_bcast.o matmul2d_jk_vec_jk_strided_1.o \
matmul2d_jk_vec_jk_strided_2.o

OBJS_utils = prepare_system.o perf_monitor.o config_json.o results_generator.o

matmul_c_params = -D N=2048 -D EPS=10E-10 -D RUN_CHECK \
-D UARCH=uarch_ivy_bridge
#matmul_asm_params = -D N=2048 -D HW_CACHELINE_SIZE=64 -D USE_IACA
matmul_asm_params = -D N=2048 -D HW_CACHELINE_SIZE=64

CFLAGS_json-glib = `pkg-config --cflags json-glib-1.0`
LDFLAGS_json-glib = `pkg-config --libs json-glib-1.0`

CFLAGS = -Wall -g -O2 -fopenmp $(CFLAGS_json-glib) $(matmul_c_params)
ASFLAGS = -m64 $(matmul_asm_params)
LDFLAGS = -lnuma $(LDFLAGS_json-glib)

all: $(OBJS_matmul) $(OBJS_utils)
		gcc $(CFLAGS) $(LDFLAGS) -o $(BIN_matmul) $^

.c.o:
		gcc $(CFLAGS) -c -o $@ $<
.S.o:
		gcc $(ASFLAGS) -c -o $@ $<

clean:
		rm -f $(BIN_matmul) $(OBJS_matmul) $(OBJS_utils)
