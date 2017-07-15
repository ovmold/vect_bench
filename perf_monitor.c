#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <linux/perf_event.h>
#include <asm/unistd.h>

#define uarch_broadwell 0
#define uarch_haswell 1
#define uarch_ivy_bridge 2

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
enum {

#define PERF_EVENT_CONFIG(name, symbol, type, config) name,

#ifndef UARCH
#warning "Default uarch is ivy_bridge."
#include "perf_events_ivy_bridge.def"
#else
    
#if UARCH == uarch_broadwell
#include "perf_events_broadwell.def"
#elif UARCH == uarch_haswell
#include "perf_events_haswell.def"
#elif UARCH == uarch_ivy_bridge
#include "perf_events_ivy_bridge.def"
#else
#error "Unknown uarch"
#endif //if UARCH ==
    
#endif //ifndef UARCH
#undef PERF_EVENT_CONFIG
    PERF_MON_RAW_MAX,
};

typedef struct perf_result_s {
    uint64_t value;
    uint64_t t_enabled;
    uint64_t t_used;
} perf_result_t;

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

perf_mon_evdesc_t perf_mon_raw_events [PERF_MON_RAW_MAX] = {

#define PERF_EVENT_CONFIG(name_, symbol_, type_, config_) \
    [name_] = {.symbol = symbol_, .type = type_, .config = config_},
#ifndef UARCH
#warning "Default uarch is ivy_bridge."
#include "perf_events_ivy_bridge.def"
#else
    
#if UARCH == uarch_broadwell
#include "perf_events_broadwell.def"
#elif UARCH == uarch_haswell
#include "perf_events_haswell.def"
#elif UARCH == uarch_ivy_bridge
#include "perf_events_ivy_bridge.def"
#else
#error "Unknown uarch"
#endif //if UARCH ==
    
#endif //ifndef UARCH
#undef PERF_EVENT_CONFIG
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

    for(int i = 0; i < PERF_MON_RAW_MAX; i++) {
        printf("Open = [%s]\n", perf_mon_raw_events[i].symbol);
        memset(&pe, 0, sizeof(struct perf_event_attr));
        pe.type = perf_mon_raw_events[i].type;
        pe.size = sizeof(struct perf_event_attr);
        pe.config = perf_mon_raw_events[i].config;
        pe.read_format = PERF_FORMAT_TOTAL_TIME_ENABLED |
            PERF_FORMAT_TOTAL_TIME_RUNNING;
        pe.disabled = 1;
        pe.exclude_kernel = 1;
        pe.exclude_hv = 1;

        perf_mon_raw_events[i].fd = perf_event_open(&pe, 0, -1, -1, 0);
        if (perf_mon_raw_events[i].fd == -1) {
            fprintf(stderr, "Error opening leader %llx\n", pe.config);
            exit(EXIT_FAILURE);
        }
    }


    for(int i = 0; i < PERF_MON_EVENT_MAX; i++ ) {
        printf("Open [%s]\n", perf_mon_events[i].symbol);
        memset(&pe, 0, sizeof(struct perf_event_attr));
        pe.type = perf_mon_events[i].type;
        pe.size = sizeof(struct perf_event_attr);
        pe.config = perf_mon_events[i].config;
        pe.read_format = PERF_FORMAT_TOTAL_TIME_ENABLED |
            PERF_FORMAT_TOTAL_TIME_RUNNING;
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
    for(int i = PERF_MON_RAW_MAX - 1; i >= 0; i-- ) {
        ioctl(perf_mon_raw_events[i].fd, PERF_EVENT_IOC_ENABLE, 0);
    }

    //    for(int i = 0; i < PERF_MON_EVENT_MAX; i++ ) {
    for(int i = PERF_MON_EVENT_MAX - 1; i >= 0; i-- ) {
        ioctl(perf_mon_events[i].fd, PERF_EVENT_IOC_RESET, 0);
    }
    //    for(int i = 0; i < PERF_MON_EVENT_MAX; i++ ) {
    for(int i = PERF_MON_RAW_MAX - 1; i >= 0; i-- ) {
        ioctl(perf_mon_raw_events[i].fd, PERF_EVENT_IOC_RESET, 0);
    }
}

void perf_mon_disable()
{
    for(int i = 0; i < PERF_MON_EVENT_MAX; i++ ) {
        ioctl(perf_mon_events[i].fd, PERF_EVENT_IOC_DISABLE, 0);
    }

    for(int i = 0; i < PERF_MON_RAW_MAX; i++ ) {
        ioctl(perf_mon_raw_events[i].fd, PERF_EVENT_IOC_DISABLE, 0);
    }
}

void perf_mon_read()
{
    perf_result_t count;
    for(int i = 0; i < PERF_MON_EVENT_MAX; i++ ) {
        read(perf_mon_events[i].fd, &count, sizeof(perf_result_t));
        printf("%-30s = %-15lu [ %-15lu / %-15lu used / enabled]\n",
               perf_mon_events[i].symbol,
               count.value, count.t_used, count.t_enabled);
        memset(&count, 0, sizeof(perf_result_t));
    }
    
    for(int i = 0; i < PERF_MON_RAW_MAX; i++ ) {
        read(perf_mon_raw_events[i].fd, &count, sizeof(perf_result_t));
        printf("%-30s = %-15lu [ %-15lu / %-15lu used / enabled]\n",
               perf_mon_raw_events[i].symbol,
               count.value, count.t_used, count.t_enabled);
        memset(&count, 0, sizeof(perf_result_t));
    }
}

void perf_mon_close()
{
   for(int i = 0; i < PERF_MON_EVENT_MAX; i++ ) {
       //        printf("Close [%s]\n", perf_mon_events[i].symbol);
        close(perf_mon_events[i].fd);
        perf_mon_events[i].fd = -1;
    }
   for(int i = 0; i < PERF_MON_RAW_MAX; i++ ) {
       //        printf("Close [%s]\n", perf_mon_events[i].symbol);
        close(perf_mon_raw_events[i].fd);
        perf_mon_raw_events[i].fd = -1;
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
