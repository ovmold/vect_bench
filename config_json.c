#include <stdlib.h>
#include <stdio.h>

#include <glib.h>

#include <json-glib/json-glib.h>

#include "perf_monitor.h"

#include "config_json.h"

static JsonParser *parser;
static JsonObject *cfg_root;
static JsonObject *result_fnames;
static JsonArray *experiments;

void config_tests(const char *conf_fname)
{
    GError *error = NULL;
    JsonNode *root;

    parser = json_parser_new ();

    json_parser_load_from_file (parser, conf_fname, &error);
    g_assert_no_error (error);

    root = json_parser_get_root (parser);
    g_assert (root != NULL);
    g_assert (JSON_NODE_HOLDS_OBJECT (root));

    cfg_root = json_node_get_object (root);
    g_assert (cfg_root != NULL);  
    g_assert (json_object_has_member (cfg_root, "result_fnames"));
    result_fnames = json_object_get_object_member (cfg_root, "result_fnames");
    g_assert (result_fnames != NULL);

    experiments = json_object_get_array_member (cfg_root, "experiments");
    g_assert (experiments);
}

long int config_get_repeats()
{
    g_assert (json_object_has_member (cfg_root, "repeat")); 

    return json_object_get_int_member (cfg_root, "repeat");
}

int config_get_experiments_number()
{
    g_assert (experiments); 

    return json_array_get_length (experiments);
}

int config_experiment(int experiment_num)
{
    
    g_assert (experiments);
    
    JsonObject *item =
        json_array_get_object_element (experiments,
                                       experiment_num);
    g_assert (item);
    if (!json_object_has_member(item, "perf_events"))
        return 1;

    JsonArray *perf_events = json_object_get_array_member(item, "perf_events");
    g_assert (perf_events);
    
    int n_events = json_array_get_length (perf_events);
    if (n_events == 0)
        return 1;
    char const **events = malloc(sizeof(char *) * n_events);
    for (int i = 0; i < n_events; i++) {
        events[i] = json_array_get_string_element(perf_events, i);
    }

    perf_mon_set_events(n_events, events);
    free(events);

    return 0;
}

void config_finalize()
{
    g_object_unref (parser);
}
