#include "perf_monitor.h"

int results_generator_begin();
int results_begin_experiment(int);
int results_begin_result(int);
int results_performance_counters(const perf_count_val_t *, int);
int results_end_result(int, double, long unsigned int, int);
int results_end_experiment(int);
int results_generator_end(const char *);
