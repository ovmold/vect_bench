#include <stdlib.h>
#include <stdio.h>

#include <glib.h>

#include <json-glib/json-glib.h>

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
    g_assert (json_object_has_member(cfg_root, "result_fnames"));
    result_fnames = json_object_get_object_member(cfg_root, "result_fnames");
    g_assert (result_fnames != NULL);

    experiments = json_object_get_array_member(cfg_root, "experiments");
    g_assert(experiments);

    printf("len(experiments) = %d\n", json_array_get_length (experiments));

}

long int config_get_repeats()
{
    g_assert (json_object_has_member(cfg_root, "repeat")); 

    return json_object_get_int_member(cfg_root, "repeat");
}

void config_finalize()
{
    g_object_unref (parser);
}
