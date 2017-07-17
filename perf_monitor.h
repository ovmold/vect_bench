#ifndef __PERF_MONITOR_H__
#define __PERF_MONITOR_H__

#include <stdint.h>

typedef struct perf_count_val_s {
    uint64_t value;
    uint64_t t_enabled;
    uint64_t t_used;
} perf_count_val_t;

int perf_mon_set_events(int events_num, const char **events);
int perf_mon_open();
void perf_mon_enable();
void perf_mon_disable();
void perf_mon_read();
void perf_mon_close();

#endif /* __PERF_MONITOR_H__ */
