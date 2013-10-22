#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ls_utils.h"
#include "ls_master.h"
#include "ls_plugin.h"


ls_plugin_t* find_entry_by_name(const char* plugin_name, ls_plugin_t* plugins, size_t num_plugins) {
    ls_plugin_t* plugin;
    for (size_t i = 0; i < num_plugins; ++i)
    {
        plugin = plugins + i;
        if (strcmp(plugin_name, plugin->plugin_name) == 0)
        {
            return plugin;
        }
    }

    return NULL;
}

int plugins_load_task_setting(ls_task_setting_t* settings, ls_plugin_t* plugins, size_t num_plugins) {
    printf("==== plugins_load_task_setting\n");

    ls_plugin_t* plugin;
    for (size_t i = 0; i < num_plugins; ++i)
    {
        plugin = plugins + i;

        Json::Value* setting = &(*setting)[plugin->plugin_name];
        // maybe Json::Value::null
        if ((plugin->task_init)(setting) < 0) {
            printf("ERROR task_init error\n");
            return -1;
        }
    }

    return 0;
}

int plugins_unload_task_setting(ls_plugin_t* plugins, size_t num_plugins) {
    printf("==== plugins_unload_task_setting()\n");

    ls_plugin_t* plugin;
    for (size_t i = 0; i < num_plugins; ++i)
    {
        plugin = plugins + i;
        if ((plugin->task_destroy)() < 0) {
            printf("ERROR failed to task_destroy()\n");
            return -1;
        }
    }

    return 0;
}

ls_plugin_api_t* find_api_entry_by_name(const char* name, ls_plugin_api_t* apis, size_t num_apis) {
    ls_plugin_api_t* api;
    for (size_t i = 0; i < num_apis; ++i)
    {
        api = apis + i;
        if (strcmp(name, api->name) == 0)
        {
            return api;
        }
    }

    return NULL;
}


// master.plugins, master.num_plugins
// 让plugin对script进行特殊处理。例如明确哪个API是属于自己的，设置好ls_api_t
int plugins_load_task_script(ls_task_script_t* script, ls_plugin_t* plugins, size_t num_plugins) {
    printf("==== plugins_load_task_script\n");

    ls_task_script_entry_t* script_entry;

    for (size_t i = 0; i < script->entries_num; ++i)
    {
        // find plugin_name of script_entry in plugins
        script_entry = script->entries + i;
        ls_plugin_t* plugin = find_entry_by_name(script_entry->plugin_name.c_str(), plugins, num_plugins);
        if (plugin == NULL)
        {
            printf("  plugin_name %s not found\n", script_entry->plugin_name.c_str());
            return -1;
        }

        // set api of script_entry in plugins
        ls_plugin_api_t* api_entry = find_api_entry_by_name(script_entry->api_name.c_str(), plugin->apis, plugin->num_apis);
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

int plugins_unload_task_script(ls_task_script_t* script, ls_plugin_t* plugins, size_t num_plugins) {
    printf("==== plugins_unload_task_script()\n");

    ls_plugin_t* plugin;
    ls_task_script_entry_t* script_entry;
    for (size_t i = 0; i < script->entries_num; ++i)
    {
        script_entry = script->entries + i;
        // find plugin_name of script_entry in plugins
        plugin = find_entry_by_name(script_entry->plugin_name.c_str(), plugins, num_plugins);
        if (plugin == NULL)
        {
            printf("  plugin_name %s not found\n", script_entry->plugin_name.c_str());
            return -1;
        }

        ls_plugin_api_t* api_entry = find_api_entry_by_name(script_entry->api_name.c_str(), plugin->apis, plugin->num_apis);
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
