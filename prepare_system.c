#include <numa.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include "prepare_system.h"

static inline pid_t gettid(void) { return syscall(__NR_gettid); }

static void print_node_mem(int node)
{
    long long free_m;
    unsigned long long size_m = numa_node_size64(node, &free_m);

    if (size_m == -1L)
        printf("node %d size: <not available>", node);
    else
        printf("node %d size: %Lu MB\n", node, size_m >> 20);

    if (free_m == -1L)
        printf("node %d free: <not available>", node);
    else
        printf("node %d free: %Lu MB\n", node, free_m >> 20); 
}

static void print_node_cpus(int node)
{
    int i, err;
    struct bitmask *cpus;

    cpus = numa_allocate_cpumask();
    err = numa_node_to_cpus(node, cpus);
    if (err >= 0) {
        printf("node %d cpus:", node);
        for (i = 0; i < cpus->size; i++)
            if (numa_bitmask_isbitset(cpus, i))
                printf(" %d", i);
    }
    printf("\n");
}

int prepare_system(char *cpu_bind, char *phys_cpu_bind, char *mem_bind)
{
    if (numa_available() == -1) {
        fprintf(stderr, "NUMA is not available.\n");
        return 1;
    }

    int maxnode = numa_max_node();
    int numnodes = 0;
    

    for (int i = 0; i <= maxnode; i++)
        if (numa_bitmask_isbitset(numa_nodes_ptr, i))
            numnodes++;

    printf("Available %d NUMA nodes\n", numnodes);

    for (int i = 0; i <= maxnode; i++) {
        if (!numa_bitmask_isbitset(numa_nodes_ptr, i))
            continue;
        print_node_cpus(i);
        print_node_mem(i);
    }

    struct bitmask *mask;
    /* membind */
    mask = numa_parse_nodestring(mem_bind);
    if (!mask) {
        printf ("<%s> is invalid\n", mem_bind);
        return 1;
    }
    
    errno = 0;
    numa_set_bind_policy(1);
    numa_set_membind(mask);
    numa_set_bind_policy(0);
    if (errno) {
        perror("setting membind");
    }
    numa_bitmask_free(mask);
    printf("Memory allocation is binded to NUMA node %s\n", mem_bind);

    mask = numa_parse_nodestring(cpu_bind);
    if (!mask) {
        printf ("<%s> is invalid\n", cpu_bind);
    }
    errno = 0;
    numa_run_on_node_mask(mask);
    if (errno) {
        perror("sched_setaffinity");
    }
    numa_bitmask_free(mask);
    printf("Run process on NUMA node %s\n", cpu_bind);


    mask = numa_parse_cpustring(phys_cpu_bind);
    if (!mask) {
        printf ("<%s> is invalid\n", phys_cpu_bind);
    }
    errno = 0;
    numa_sched_setaffinity(0, mask);
    if (errno) {
        perror("sched_setaffinity");
    }
    numa_bitmask_free(mask);
    
    return 0;
}
