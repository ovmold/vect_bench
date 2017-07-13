
int perf_mon_open();
void perf_mon_enable();
void perf_mon_disable();
void perf_mon_read();
void perf_mon_close();

#ifdef USE_PERF
#warning "Perf is used."
#define PERF_MON_OPEN() perf_mon_open()
#define PERF_MON_ENABLE() perf_mon_enable()
#define PERF_MON_DISABLE() perf_mon_disable()
#define PERF_MON_READ() perf_mon_read()
#define PERF_MON_CLOSE() perf_mon_close()
#else
#define PERF_MON_OPEN()
#define PERF_MON_ENABLE()
#define PERF_MON_DISABLE()
#define PERF_MON_READ()
#define PERF_MON_CLOSE()
#endif
