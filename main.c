#include <stdio.h>

#include <glib.h>

#include "config_json.h"
#include "prepare_system.h"
#include "perf_monitor.h"
#include "matmul.h"

const char *matmul_config = "experiments-config.json";

int main(int argc, char* argv[])
{
    g_test_init (&argc, &argv, NULL);
    config_tests(matmul_config);
    prepare_system("1", "23", "1");
    
    for (int i = 0; i < matmul2d_number_items; i++) {
        test_run(i);
    }
    
    config_finalize();
    
    return 0;
}
