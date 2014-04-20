#pragma once

typedef struct {
    int workers_num;
    size_t plugins_num;
    char** plugin_paths;
} ls_config_t;

int load_config(ls_config_t* config);
int unload_config(ls_config_t* config);
