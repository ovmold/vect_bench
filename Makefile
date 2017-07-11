BIN_matmul = matmul
OBJS_matmul = main.o prepare_system.o matmul2d.o \
matmul2d_jk_novec.o matmul2d_kj_novec.o \
matmul2d_kj_vec_k.o matmul2d_kj_vec_k_no_gather.o matmul2d_kj_vec_j.o \
matmul2d_kj_vec_kj_1.o matmul2d_kj_vec_kj_1_no_gather.o matmul2d_kj_vec_kj_2.o \
matmul2d_jk_vec_k_gather.o matmul2d_jk_vec_k_no_gather.o matmul2d_jk_vec_j.o \
matmul2d_jk_vec_jk_bcast.o matmul2d_jk_vec_jk_strided_1.o

CFLAGS = -Wall -g -O1 -fopenmp -D N=2048 -D EPS=10E-10
#ASFLAGS = -m64 -D N=2048 -D HW_CACHELINE_SIZE=64
ASFLAGS = -m64 -D N=2048 -D HW_CACHELINE_SIZE=64 -D USE_IACA
LDFLAGS = -lnuma

all: $(OBJS_matmul)
		gcc $(CFLAGS) $(LDFLAGS) -o $(BIN_matmul) $^

.c.o:
		gcc $(CFLAGS) -c -o $@ $<
.S.o:
		gcc $(ASFLAGS) -c -o $@ $<

clean:
		rm -f $(BIN_matmul) $(OBJS_matmul)
