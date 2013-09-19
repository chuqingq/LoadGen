
#include "ls_config.h"

int read_config(ls_config_t* config) {
    /* TODO */
    config->port = 7879;
    config->worker_num = 4;
    return 0;
}
