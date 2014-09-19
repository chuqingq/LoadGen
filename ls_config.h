#pragma once

typedef struct {
    int workers_num;
} ls_config_t;

int load_config(ls_config_t* config);
int unload_config(ls_config_t* config);
