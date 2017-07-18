#include <stdlib.h>
#include <stdio.h>

#include <glib.h>

#include <json-glib/json-glib.h>

#include "perf_monitor.h"
#include "config_json.h"

#include "results_generator.h"

JsonBuilder *builder = NULL;

int results_generator_begin()
{
    builder = json_builder_new ();
    json_builder_begin_array (builder);
    return 0;
}

int results_begin_experiment(int id)
{
    const char *name = config_get_experiment_name (id);

    json_builder_begin_object (builder);
    json_builder_set_member_name (builder, "name");
    json_builder_add_string_value (builder, name);
 
    json_builder_set_member_name (builder, "run_results");
    json_builder_begin_array (builder);
    return 0;
}

int results_begin_result(int id)
{
    json_builder_begin_object (builder);
    
    json_builder_set_member_name (builder, "id");
    json_builder_add_int_value (builder, id);

    return 0;
}

int results_performance_counters(const perf_count_val_t *counts, int counts_num)
{
    json_builder_set_member_name (builder, "performance counters");
    json_builder_begin_array (builder);

    for (int i = 0; i < counts_num; i++) {
        json_builder_begin_object (builder);
    
        json_builder_set_member_name (builder, "enabled");
        json_builder_add_int_value (builder, counts[i].t_enabled);
        json_builder_set_member_name (builder, "value");
        json_builder_add_int_value (builder, counts[i].value);
        json_builder_set_member_name (builder, "used");
        json_builder_add_int_value (builder, counts[i].t_used);

        json_builder_end_object (builder);
    }

    json_builder_end_array (builder);

    return 0;
}

int results_end_result(int id, double t, long unsigned int tsc, int status)
{
    json_builder_set_member_name (builder, "time");
    json_builder_add_double_value (builder, t);

    json_builder_set_member_name (builder, "tsc_val");
    json_builder_add_int_value (builder, tsc);

    json_builder_set_member_name (builder, "check");
    json_builder_add_int_value (builder, status ? TRUE : FALSE);
    
    json_builder_end_object (builder);

    return 0;
}

int results_end_experiment(int id)
{
    json_builder_end_array (builder);
    json_builder_end_object (builder);

    return 0;
}

int results_generator_end(const char *test_name)
{
    JsonGenerator *gen = json_generator_new();
    JsonNode *root = NULL;
    GError *error = NULL;
    const char *fname = config_get_result_fname (test_name);
    
    json_builder_end_array (builder);
    
    root = json_builder_get_root (builder);
    json_generator_set_root (gen, root);
    json_generator_set_pretty(gen, TRUE);
    json_generator_set_indent(gen, 1);
    json_generator_set_indent_char(gen, '\t');
    if (!json_generator_to_file (gen, fname, &error)) {
        g_print ("Error: %s\n", error->message);
        g_error_free (error);
        exit (EXIT_FAILURE);
    }


    json_node_free (root);
    g_clear_object (&gen);

    return 0;
}
