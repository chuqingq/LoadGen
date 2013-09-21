#include "ls_config.h"

ls_config_t config;

int load_config(ls_config_t* config) {
    // config->port = 7879;
    config->worker_num = 4;

    config->plugin_paths.push_back("plugin/plugin_demo/libplugin_demo.so");

    return 0;
}
