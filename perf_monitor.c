#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <linux/perf_event.h>
#include <asm/unistd.h>

enum {
    PERF_MON_INSTRUCTIONS,
    PEF_MON_L1_DCACHE_LOAD_MISSES,
    PERF_MON_L1_DCACHE_LOADS,
    PERF_MON_L1_DCACHE_STORE_MISSES,
    PERF_MON_L1_DCACHE_STORES,
    PERF_MON_LLC_LOADS,
    PERF_MON_LLC_STORES,
    PERF_MON_DTLB_LOAD_MISSES,
    PERF_MON_DTLB_LOADS,
    PERF_MON_DTLB_STORE_MISSES,
    PERF_MON_DTLB_STORES,
    PERF_MON_EVENT_MAX,
};

typedef struct perf_mon_evdesc_s {
    const char *symbol;
    uint32_t type;
    uint64_t config;
    int fd;
} perf_mon_evdesc_t;

perf_mon_evdesc_t perf_mon_events [PERF_MON_EVENT_MAX] = {
    [PERF_MON_INSTRUCTIONS] =
    {
        .symbol = "instructions",
        .type = PERF_TYPE_HARDWARE,
        .config = PERF_COUNT_HW_CPU_CYCLES,
    },
    
    [PEF_MON_L1_DCACHE_LOAD_MISSES] =
    {
        .symbol = "L1-dcache-load-misses",
        .type = PERF_TYPE_HW_CACHE,
        .config =
        PERF_COUNT_HW_CACHE_L1D		<<  0  |
        (PERF_COUNT_HW_CACHE_OP_READ		<<  8) |
        (PERF_COUNT_HW_CACHE_RESULT_MISS	<< 16),
    },
    
    [PERF_MON_L1_DCACHE_LOADS] =
    {
        .symbol = "L1-dcache-loads",
        .type = PERF_TYPE_HW_CACHE,
        .config =
        PERF_COUNT_HW_CACHE_L1D		<<  0  |
        (PERF_COUNT_HW_CACHE_OP_READ		<<  8) |
        (PERF_COUNT_HW_CACHE_RESULT_ACCESS	<< 16),
    },
    
    [PERF_MON_L1_DCACHE_STORE_MISSES] =
    {
        .symbol = "L1-dcache-store-misses",
        .type = PERF_TYPE_HW_CACHE,
        .config =
        PERF_COUNT_HW_CACHE_L1D		<<  0  |
        (PERF_COUNT_HW_CACHE_OP_WRITE		<<  8) |
        (PERF_COUNT_HW_CACHE_RESULT_MISS	<< 16),
    },
    
    [PERF_MON_L1_DCACHE_STORES] =
    {
        .symbol = "L1-dcache-stores",
        .type = PERF_TYPE_HW_CACHE,
        .config =
        PERF_COUNT_HW_CACHE_L1D		<<  0  |
        (PERF_COUNT_HW_CACHE_OP_WRITE		<<  8) |
        (PERF_COUNT_HW_CACHE_RESULT_ACCESS	<< 16),
    },
    
    [PERF_MON_LLC_LOADS] =
    {
        .symbol = "LLC-loads",
        .type = PERF_TYPE_HW_CACHE,
        .config =
        PERF_COUNT_HW_CACHE_LL		<<  0  |
        (PERF_COUNT_HW_CACHE_OP_READ		<<  8) |
        (PERF_COUNT_HW_CACHE_RESULT_ACCESS	<< 16),
    },
    
    [PERF_MON_LLC_STORES] =
    {
        .symbol = "LLC-stores",
        .type = PERF_TYPE_HW_CACHE,
        .config =
        PERF_COUNT_HW_CACHE_LL		<<  0  |
        (PERF_COUNT_HW_CACHE_OP_WRITE		<<  8) |
        (PERF_COUNT_HW_CACHE_RESULT_ACCESS	<< 16),
    },
    
    [PERF_MON_DTLB_LOAD_MISSES] =
    {
        .symbol = "dTLB-load-misses",
        .type = PERF_TYPE_HW_CACHE,
        .config =
        PERF_COUNT_HW_CACHE_DTLB		<<  0  |
        (PERF_COUNT_HW_CACHE_OP_READ		<<  8) |
        (PERF_COUNT_HW_CACHE_RESULT_MISS	<< 16),
    },
    
    [PERF_MON_DTLB_LOADS] =
    {
        .symbol = "dTLB-loads",
        .type = PERF_TYPE_HW_CACHE,
        .config =
        PERF_COUNT_HW_CACHE_DTLB		<<  0  |
        (PERF_COUNT_HW_CACHE_OP_READ		<<  8) |
        (PERF_COUNT_HW_CACHE_RESULT_ACCESS	<< 16),
    },
    
    [PERF_MON_DTLB_STORE_MISSES] =
    {
        .symbol = "dTLB-store-misses",
        .type = PERF_TYPE_HW_CACHE,
        .config =
        PERF_COUNT_HW_CACHE_DTLB		<<  0  |
        (PERF_COUNT_HW_CACHE_OP_WRITE		<<  8) |
        (PERF_COUNT_HW_CACHE_RESULT_MISS	<< 16),
    },
    
    [PERF_MON_DTLB_STORES] =
    {
        .symbol = "dTLB-stores",
        .type = PERF_TYPE_HW_CACHE,
        .config =
        PERF_COUNT_HW_CACHE_DTLB		<<  0  |
        (PERF_COUNT_HW_CACHE_OP_WRITE		<<  8) |
        (PERF_COUNT_HW_CACHE_RESULT_ACCESS	<< 16),
    },
};

static long perf_event_open(struct perf_event_attr *hw_event, pid_t pid,
                            int cpu, int group_fd, unsigned long flags)
{
    int ret;

    ret = syscall(__NR_perf_event_open, hw_event, pid, cpu,
                  group_fd, flags);
    return ret;
}

int perf_mon_open()
{
    struct perf_event_attr pe;
    for(int i = 0; i < PERF_MON_EVENT_MAX; i++ ) {
        printf("Open [%s]\n", perf_mon_events[i].symbol);
        memset(&pe, 0, sizeof(struct perf_event_attr));
        pe.type = perf_mon_events[i].type;
        pe.size = sizeof(struct perf_event_attr);
        pe.config = perf_mon_events[i].config;
        pe.disabled = 1;
        pe.exclude_kernel = 1;
        pe.exclude_hv = 1;

        perf_mon_events[i].fd = perf_event_open(&pe, 0, -1, -1, 0);
        if (perf_mon_events[i].fd == -1) {
            fprintf(stderr, "Error opening leader %llx\n", pe.config);
            exit(EXIT_FAILURE);
        }
    }

    return 0;
}

void perf_mon_enable()
{
    for(int i = PERF_MON_EVENT_MAX - 1; i >= 0; i-- ) {
        ioctl(perf_mon_events[i].fd, PERF_EVENT_IOC_ENABLE, 0);
    }
    //    for(int i = 0; i < PERF_MON_EVENT_MAX; i++ ) {
    for(int i = PERF_MON_EVENT_MAX - 1; i >= 0; i-- ) {
        ioctl(perf_mon_events[i].fd, PERF_EVENT_IOC_RESET, 0);
    }
}

void perf_mon_disable()
{
    for(int i = 0; i < PERF_MON_EVENT_MAX; i++ ) {
        ioctl(perf_mon_events[i].fd, PERF_EVENT_IOC_DISABLE, 0);
    }
}

void perf_mon_read()
{
    long long count;
    for(int i = 0; i < PERF_MON_EVENT_MAX; i++ ) {
        read(perf_mon_events[i].fd, &count, sizeof(long long));        
        printf("%s = %lld\n", perf_mon_events[i].symbol, count);
        count = 0;
    }
}

void perf_mon_close()
{
   for(int i = 0; i < PERF_MON_EVENT_MAX; i++ ) {
       //        printf("Close [%s]\n", perf_mon_events[i].symbol);
        close(perf_mon_events[i].fd);
        perf_mon_events[i].fd = -1;
    }

}

void perf_mon_empty_test()
{
    perf_mon_open();
    perf_mon_enable();
    perf_mon_disable();
    perf_mon_read();
    perf_mon_close();
}
