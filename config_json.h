
void config_tests(const char *conf_fname);

long int config_get_repeats();
int config_get_experiments_number();
const char *config_get_experiment_name(int);
const char *config_get_result_fname(const char *);
int config_experiment(int experiment_num);

void config_finalize();
