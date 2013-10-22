#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ls_utils.h"
#include "ls_master.h"
#include "ls_plugin.h"

int load_plugins() {
    printf("==== load_plugins()\n");

    master.num_plugins = master.config.plugins_num;

    // allocate master.plugins
    master.plugins = (ls_plugin_entry_t*)malloc(master.num_plugins * sizeof(ls_plugin_entry_t));

    ls_plugin_entry_t* entry;
    for (size_t i = 0; i < master.num_plugins; ++i)
    {
        entry = master.plugins + i;
        if (uv_dlopen(master.config.plugin_paths[i], &(entry->plugin_lib)) < 0) {
            printf("  Failed to uv_dlopen\n");
            return -1;
        }

        if (uv_dlsym(&(entry->plugin_lib), "plugin_declare", (void**)&(entry->plugin_declare)) < 0) {
            printf("  Failed to uv_dlsym\n");
            return -1;
        }

        if ((entry->plugin_declare(entry) < 0)) {
            printf("  Failed to plugin_declare\n");
            return -1;
        }

        if ((entry->plugin_load)() < 0)
        {
            printf("ERROR failed to plugin_load()\n");
            return -1;
        }
    }

    return 0;
}

int unload_plugins() {
    printf("==== unload_plugins()\n");

    ls_plugin_entry_t* entry;
    for (size_t i = 0; i < master.num_plugins; ++i)
    {
        entry = master.plugins + i;
        printf("  plugin=%s\n", entry->plugin_name);

        // 1. 调用plugin_unload()
        if ((entry->plugin_unload)() < 0)
        {
            printf("ERROR failed to plugin_unload()\n");
            // return -1;// 确保所有插件都正常卸载
        }
        uv_dlclose(&entry->plugin_lib);
    }

    free(master.plugins);
    master.plugins = NULL;
    master.num_plugins = 0;

    return 0;
}


ls_plugin_entry_t* find_entry_by_name(const char* plugin_name, ls_plugin_entry_t* plugins, size_t num_plugins) {
    ls_plugin_entry_t* entry;
    for (size_t i = 0; i < num_plugins; ++i)
    {
        entry = plugins + i;
        if (strcmp(plugin_name, entry->plugin_name) == 0)
        {
            return entry;
        }
    }

    return NULL;
}

int plugins_load_task_setting(ls_task_setting_t* settings, ls_plugin_entry_t* plugins, size_t num_plugins) {
    printf("==== plugins_load_task_setting\n");

    ls_plugin_entry_t* entry;
    for (size_t i = 0; i < num_plugins; ++i)
    {
        entry = plugins + i;

        Json::Value* setting = &(*setting)[entry->plugin_name];
        // maybe Json::Value::null
        if ((entry->task_init)(setting) < 0) {
            printf("ERROR task_init error\n");
            return -1;
        }
    }

    return 0;
}

int plugins_unload_task_setting(ls_plugin_entry_t* plugins, size_t num_plugins) {
    printf("==== plugins_unload_task_setting()\n");

    ls_plugin_entry_t* entry;
    for (size_t i = 0; i < num_plugins; ++i)
    {
        entry = plugins + i;
        if ((entry->task_destroy)() < 0) {
            printf("ERROR failed to task_destroy()\n");
            return -1;
        }
    }

    return 0;
}

ls_plugin_api_t* find_api_entry_by_name(const char* name, ls_plugin_api_t* apis, size_t num_apis) {
    ls_plugin_api_t* entry;
    for (size_t i = 0; i < num_apis; ++i)
    {
        entry = apis + i;
        if (strcmp(name, entry->name) == 0)
        {
            return entry;
        }
    }

    return NULL;
}


// master.plugins, master.num_plugins
// 让plugin对script进行特殊处理。例如明确哪个API是属于自己的，设置好ls_api_t
int plugins_load_task_script(ls_task_script_t* script, ls_plugin_entry_t* plugins, size_t num_plugins) {
    printf("==== plugins_load_task_script\n");

    ls_task_script_entry_t* script_entry;

    for (size_t i = 0; i < script->entries_num; ++i)
    {
        // find plugin_name of script_entry in plugins
        script_entry = script->entries + i;
        ls_plugin_entry_t* entry = find_entry_by_name(script_entry->plugin_name.c_str(), plugins, num_plugins);
        if (entry == NULL)
        {
            printf("  plugin_name %s not found\n", script_entry->plugin_name.c_str());
            return -1;
        }

        // set api of script_entry in plugins
        ls_plugin_api_t* api_entry = find_api_entry_by_name(script_entry->api_name.c_str(), entry->apis, entry->num_apis);
        if (api_entry == NULL)
        {
            printf("  api %s not found\n", script_entry->api_name.c_str());
            return -1;
        }

        void* args = NULL;
        if ((api_entry->init)(&script_entry->json_args, &args) < 0)
        {
            printf("  api %s init error\n", script_entry->api_name.c_str());
            return -1;
        }

        // 设置api和args
        script_entry->api = (void*)api_entry->run;
        script_entry->args = args;
    }

    return 0;
}

int plugins_unload_task_script(ls_task_script_t* script, ls_plugin_entry_t* plugins, size_t num_plugins) {
    printf("==== plugins_unload_task_script()\n");

    ls_plugin_entry_t* entry;
    ls_task_script_entry_t* script_entry;
    for (size_t i = 0; i < script->entries_num; ++i)
    {
        script_entry = script->entries + i;
        // find plugin_name of script_entry in plugins
        entry = find_entry_by_name(script_entry->plugin_name.c_str(), plugins, num_plugins);
        if (entry == NULL)
        {
            printf("  plugin_name %s not found\n", script_entry->plugin_name.c_str());
            return -1;
        }

        ls_plugin_api_t* api_entry = find_api_entry_by_name(script_entry->api_name.c_str(), entry->apis, entry->num_apis);
        if (api_entry == NULL)
        {
            printf("  api %s not found\n", script_entry->api_name.c_str());
            return -1;
        }

        if ((api_entry->destroy)(&script_entry->args) < 0)
        {
            printf("  api %s destroy error\n", script_entry->api_name.c_str());
            return -1;
        }
    }

    return 0;
}
