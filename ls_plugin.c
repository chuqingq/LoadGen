#include "ls_plugin.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ls_utils.h"
#include "ls_master.h"
#include "ls_task_script.h"
#include "ls_task_setting.h"


ls_plugin_api_t* find_api_by_name(const char* name, ls_plugin_t* plugins, size_t num_plugins) {
    ls_plugin_t* plugin;
    for (size_t i = 0; i < num_plugins; ++i) {
        plugin = plugins + i;
        ls_plugin_api_t* api;
        for (size_t j = 0; j < plugin->num_apis; ++j) {
            api = plugin->apis + j;
            if (strcmp(name, api->name) == 0) {
                return api;
            }
        }
    }
    return NULL;
}


int plugins_task_init(ls_plugin_t* plugins, size_t num_plugins) {
    LOG("plugins_task_init\n");

    ls_plugin_t* plugin;
    for (size_t i = 0; i < num_plugins; ++i)
    {
        plugin = plugins + i;

        if (plugin->task_init != NULL && (plugin->task_init)() < 0) {
            LOG("ERROR failed to %s.task_init()\n", plugin->plugin_name);
            return -1;
        }
    }

    return 0;
}

int plugins_task_terminate(ls_plugin_t* plugins, size_t num_plugins) {
    LOG("plugins_unload_task_setting()\n");

    ls_plugin_t* plugin;
    for (size_t i = 0; i < num_plugins; ++i)
    {
        plugin = plugins + i;
        if (plugin->task_terminate != NULL && (plugin->task_terminate)() < 0) {
            LOG("ERROR failed to %s.task_terminate()\n", plugin->plugin_name);
            return -1;
        }
    }

    return 0;
}
