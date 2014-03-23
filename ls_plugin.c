#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ls_utils.h"
#include "ls_master.h"
#include "ls_plugin.h"
#include "ls_task_script.h"
#include "ls_task_setting.h"


static ls_plugin_t* find_entry_by_name(const char* plugin_name, ls_plugin_t* plugins, size_t num_plugins) {
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

static ls_plugin_api_t* find_api_entry_by_name(const char* name, ls_plugin_api_t* apis, size_t num_apis) {
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
// int plugins_load_task_script(ls_task_script_t* script, ls_plugin_t* plugins, size_t num_plugins) {
int plugins_script_init(ls_task_setting_t* setting,
                        ls_task_script_t* script,
                        ls_plugin_t* plugins,
                        size_t num_plugins) {
    LOG("plugins_script_init\n");

    // 遍历所有plugins，执行script_init()
    ls_plugin_t* plugin;
    for (size_t i = 0; i < num_plugins; ++i)
    {
        plugin = plugins + i;
        if (plugin->script_init != NULL && (plugin->script_init)(setting) < 0) {
            LOG("ERROR failed to %s.script_init()\n", plugin->plugin_name);
            return -1;
        }
    }

    // 遍历所有script所有项，执行api.init()
    ls_task_script_entry_t* script_entry;
    for (size_t i = 0; i < script->entries_num; ++i)
    {
        // find plugin_name of script_entry in plugins
        script_entry = script->entries + i;
        ls_plugin_t* plugin = find_entry_by_name(script_entry->plugin_name.c_str(), plugins, num_plugins);
        if (plugin == NULL)
        {
            LOG("  plugin %s not found\n", script_entry->plugin_name.c_str());
            return -1;
        }

        // set api of script_entry in plugins
        // ls_plugin_api_t* api_entry = find_api_entry_by_name(script_entry->api_name.c_str(), plugin->apis, plugin->num_apis);
        script_entry->api = find_api_entry_by_name(script_entry->api_name.c_str(), plugin->apis, plugin->num_apis);
        if (script_entry->api == NULL)
        {
            LOG("  api %s not found\n", script_entry->api_name.c_str());
            return -1;
        }

        if ((script_entry->api->init)((const JSONNODE**) &script_entry->json_args, &script_entry->args) < 0)
        {
            LOG("  api %s init error\n", script_entry->api_name.c_str());
            return -1;
        }
    }

    return 0;
}

int plugins_script_terminate(ls_task_setting_t* setting,
                             ls_task_script_t* script,
                             ls_plugin_t* plugins,
                             size_t num_plugins) {
    LOG("plugins_script_terminate()\n");

    // 遍历所有plugins，执行script_terminate()
    ls_plugin_t* plugin;
    for (size_t i = 0; i < num_plugins; ++i)
    {
        plugin = plugins + i;
        if (plugin->script_terminate != NULL && (plugin->script_terminate)(setting) < 0) {
            LOG("ERROR failed to %s.script_terminate()\n", plugin->plugin_name);
            return -1;
        }
    }

    // 遍历所有script所有项，执行api.terminate()
    ls_task_script_entry_t* script_entry;
    for (size_t i = 0; i < script->entries_num; ++i)
    {
        script_entry = script->entries + i;

        if (script_entry->api == NULL)
        {
            LOG("  api %s not found\n", script_entry->api_name.c_str());
            return -1;
        }

        if ((script_entry->api->terminate)(&script_entry->args) < 0)
        {
            LOG("  api %s terminate error\n", script_entry->api_name.c_str());
            return -1;
        }
    }

    return 0;
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
