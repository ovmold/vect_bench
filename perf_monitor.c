#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <linux/perf_event.h>
#include <asm/unistd.h>

#include "results_generator.h"

#define uarch_broadwell 0
#define uarch_haswell 1
#define uarch_ivy_bridge 2

enum {
    PERF_MON_CYCLES,
    PERF_MON_INSTRUCTIONS,
    PERF_MON_BRANCH_INSTRUCTIONS,
    PERF_MON_BRANCH_MISSES,
    PERF_MON_L1_DCACHE_LOAD_MISSES,
    PERF_MON_L1_DCACHE_LOADS,
    PERF_MON_L1_DCACHE_STORE_MISSES,
    PERF_MON_L1_DCACHE_STORES,
    PERF_MON_LLC_LOADS_MISSES,    
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

typedef struct perf_mon_evdesc_s {
    const char *symbol;
    uint32_t type;
    uint64_t config;
} perf_mon_evdesc_t;

typedef struct perf_mon_experiment_s {
    int n_events;
    int *fd;   
    perf_mon_evdesc_t **used_events;
    perf_count_val_t *counters;
} perf_mon_experiment_t;

perf_mon_evdesc_t perf_mon_events [PERF_MON_EVENT_MAX] = {
    [PERF_MON_CYCLES] =
    {
        .symbol = "cycles",
        .type = PERF_TYPE_HARDWARE,
        .config = PERF_COUNT_HW_CPU_CYCLES,
    },
    [PERF_MON_INSTRUCTIONS] =
    {
        .symbol = "instructions",
        .type = PERF_TYPE_HARDWARE,
        .config = PERF_COUNT_HW_INSTRUCTIONS,
    },

    [PERF_MON_BRANCH_INSTRUCTIONS] =
    {
        .symbol = "branch-instructions",
        .type = PERF_TYPE_HARDWARE,
        .config = PERF_COUNT_HW_BRANCH_INSTRUCTIONS,
    },
    
    [PERF_MON_BRANCH_MISSES] =
    {
        .symbol = "branch-misses",
        .type = PERF_TYPE_HARDWARE,
        .config = PERF_COUNT_HW_BRANCH_MISSES,
    },
    
    [PERF_MON_L1_DCACHE_LOAD_MISSES] =
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
    
    [PERF_MON_LLC_LOADS_MISSES] =
    {
        .symbol = "LLC-load-misses",
        .type = PERF_TYPE_HW_CACHE,
        .config =
        PERF_COUNT_HW_CACHE_LL		<<  0  |
        (PERF_COUNT_HW_CACHE_OP_READ		<<  8) |
        (PERF_COUNT_HW_CACHE_RESULT_MISS	<< 16),
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

static perf_mon_experiment_t experiment;

static long perf_event_open(struct perf_event_attr *hw_event, pid_t pid,
                            int cpu, int group_fd, unsigned long flags)
{
    int ret;

    ret = syscall(__NR_perf_event_open, hw_event, pid, cpu,
                  group_fd, flags);
    return ret;
}

static perf_mon_evdesc_t *lookup_evdesc(const char *ev_name)
{
    for (int i = 0; i < PERF_MON_RAW_MAX; i++) {
        if (!strcmp(ev_name, perf_mon_raw_events[i].symbol))
            return &perf_mon_raw_events[i];
    }

    for (int i = 0; i < PERF_MON_EVENT_MAX; i++) {
        if (!strcmp(ev_name, perf_mon_events[i].symbol))
            return &perf_mon_events[i];
    }
    return NULL;
}

int perf_mon_set_events(int events_num, const char **events)
{
    experiment.n_events = events_num;
    experiment.used_events = malloc(sizeof(perf_mon_evdesc_t *) * events_num);
    experiment.fd = malloc(sizeof(int) * events_num);
    experiment.counters = malloc(sizeof(perf_count_val_t) * events_num);

    if (experiment.used_events == NULL || experiment.fd == NULL ||
        experiment.counters == NULL) {
        fprintf(stderr, "[%s:%d] malloc error\n", __func__, __LINE__);
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < events_num; i++ ) {
        if ((experiment.used_events[i] = lookup_evdesc(events[i])) == NULL) {
            fprintf(stderr, "[%s:%d] Unknown events \"%s\"\n",
                    __func__, __LINE__, events[i]);
        }
    }
    return 0;
}

int perf_mon_open()
{
    perf_mon_evdesc_t **events = experiment.used_events;
    int *fds = experiment.fd;
    struct perf_event_attr pe;

    for (int i = 0; i < experiment.n_events; i++) {
        if (events[i] == NULL)
            continue;
        
        printf("Open = [%s]\n", events[i]->symbol);
        memset(&pe, 0, sizeof(struct perf_event_attr));
        pe.type = events[i]->type;
        pe.size = sizeof(struct perf_event_attr);
        pe.config = events[i]->config;
        pe.read_format = PERF_FORMAT_TOTAL_TIME_ENABLED |
            PERF_FORMAT_TOTAL_TIME_RUNNING;
        pe.disabled = 1;
        pe.exclude_kernel = 1;
        pe.exclude_hv = 1;

        fds[i] = perf_event_open(&pe, 0, -1, -1, 0);
        if (fds[i] == -1) {
            fprintf(stderr, "Error opening leader %llx\n", pe.config);
            exit(EXIT_FAILURE);
        }
    }

    return 0;
}

void perf_mon_enable()
{
    int *fds = experiment.fd;
    
    for (int i = 0; i < experiment.n_events; i++) {
        ioctl(fds[i], PERF_EVENT_IOC_ENABLE, 0);
    }
    for (int i = 0; i < experiment.n_events; i++) {
        ioctl(fds[i], PERF_EVENT_IOC_RESET, 0);
    }
}

void perf_mon_disable()
{
    int *fds = experiment.fd;
    
    for (int i = 0; i < experiment.n_events; i++) {
        ioctl(fds[i], PERF_EVENT_IOC_DISABLE, 0);
    }
}

void perf_mon_read()
{
    int *fds = experiment.fd;
    perf_count_val_t *counts = experiment.counters;

    for (int i = 0; i < experiment.n_events; i++) {
        read(fds[i], &counts[i], sizeof(perf_count_val_t));
        printf("%-30s = %-15lu [ %-15lu / %-15lu used / enabled]\n",
               experiment.used_events[i]->symbol,
               counts[i].value, counts[i].t_used, counts[i].t_enabled);
        
    }
    results_performance_counters(counts, experiment.n_events);
}

void perf_mon_close()
{
    int *fds = experiment.fd;
    
    for (int i = 0; i < experiment.n_events; i++) {
        close(fds[i]);
        fds[i] = -1;
    }
   
    free(experiment.used_events);
    free(experiment.fd);
    free(experiment.counters);

    experiment.used_events = NULL;
    experiment.fd = NULL;
    experiment.counters = NULL;
    experiment.n_events = 0;
}

void perf_mon_empty_test()
{
    perf_mon_open();
    perf_mon_enable();
    perf_mon_disable();
    perf_mon_read();
    perf_mon_close();
}
