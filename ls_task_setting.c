#include "ls_task_setting.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "lib/libjson/include/libjson.h"
#include "ls_master.h"
#include "ls_utils.h"

// master不保存settings，直接传给plugin_init()
int load_task_setting(ls_master_t* master) {
    LOG("load_task_setting()\n");
    const char* setting_file = "task/setting.json";

    char* buf;
    long len;
    FILE* f = fopen(setting_file, "r");
    if (f == NULL) {
        LOGE("Failed to open setting_file: %s\n", setting_file);// TODO errno
        return -1;
    }

    fseek(f, 0, SEEK_END);
    len = ftell(f);
    rewind(f);

    buf = (char*) malloc(len + 1);
    if (NULL == buf) {
        LOGE("Failed to malloc for task_setting: %s\n", strerror(errno));
        return -1;
    }

    len = fread(buf, 1, len, f);
    buf[len] = '\0';

    JSONNODE* setting = json_parse(buf);

    master->num_plugins = json_size(setting);
    master->plugins = (ls_plugin_t*)malloc(master->num_plugins * sizeof(ls_plugin_t));

    ls_plugin_t* plugin;
    size_t plugin_index = 0;
    char plugin_path[128];
    for (JSONNODE_ITERATOR i = json_begin(setting); i != json_end(setting); ++i, ++plugin_index) {
        plugin = master->plugins + plugin_index;
        plugin->plugin_index = plugin_index;

        json_char* plugin_name = json_name(*i);
        if (NULL == plugin_name) {
            LOGE("Failed to get plugin_name from task_setting\n");
            return -1;
        }

        snprintf(plugin_path, sizeof(plugin_path), "plugin/%s/%s.so", plugin_name, plugin_name);

        if (uv_dlopen(plugin_path, &plugin->plugin_lib) < 0) {
            LOGE("  Failed to uv_dlopen: %s\n", uv_dlerror(&plugin->plugin_lib));
            return -1;
        }

        if (uv_dlsym(&(plugin->plugin_lib), "plugin_declare", (void**)&(plugin->plugin_declare)) < 0) {
            LOGE("  Failed to uv_dlsym\n");
            return -1;
        }

        if ((plugin->plugin_declare(plugin) < 0)) {
            LOGE("  Failed to plugin_declare\n");
            return -1;
        }

        JSONNODE* settings = *i;
        // if (plugin->master_init != NULL && (plugin->master_init)(master, settings) < 0)
        if (plugin->plugin_init != NULL && (plugin->plugin_init)(settings) < 0) {
            LOGE("ERROR failed to plugin_init()\n");
            return -1;
        }
    }

    // TODO json_free
    return 0;
}

int unload_task_setting(ls_master_t* master) {
    ls_plugin_t* plugin;
    for (size_t i = 0; i < master->num_plugins; ++i) {
        plugin = master->plugins + i;
        // if (NULL != plugin->master_terminate && (plugin->master_terminate)(master) < 0)
        if (NULL != plugin->plugin_terminate && (plugin->plugin_terminate)() < 0) {
            LOGE("ERROR failed to plugin_terminate()\n");
        }
    }

    return 0;
}
