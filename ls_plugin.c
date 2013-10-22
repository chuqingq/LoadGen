#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ls_utils.h"
#include "ls_master.h"
#include "ls_plugin.h"

int load_plugins(ls_plugin_t* plugins) {
    printf("==== load_plugins()\n");

    plugins->entries_num = master.config.plugins_num;

    // 加载plugin目录下的插件
    ls_master_t* master = container_of(plugins, ls_master_t, plugins);

    // allocate master.plugins
    plugins->entries = (ls_plugin_entry_t*)malloc(plugins->entries_num * sizeof(ls_plugin_entry_t));

    ls_plugin_entry_t* entry;
    for (size_t i = 0; i < plugins->entries_num; ++i)
    {
        entry = &plugins->entries[i];
        if (uv_dlopen(master->config.plugin_paths[i], &(entry->plugin_lib)) < 0) {
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

int unload_plugins(ls_plugin_t* plugins) {
    printf("==== unload_plugins()\n");

    ls_plugin_entry_t* entry;
    for (size_t i = 0; i < plugins->entries_num; ++i)
    {
        entry = &plugins->entries[i];
        printf("  plugin=%s\n", entry->plugin_name);

        // 1. 调用plugin_unload()
        if ((entry->plugin_unload)() < 0)
        {
            printf("ERROR failed to plugin_unload()\n");
            // return -1;// 确保所有插件都正常卸载
        }
        uv_dlclose(&entry->plugin_lib);
    }

    free(plugins->entries);
    plugins->entries = NULL;
    return 0;
}


ls_plugin_entry_t* find_entry_by_name(const char* plugin_name, ls_plugin_t* plugins) {
    ls_plugin_entry_t* entry;
    for (size_t i = 0; i < plugins->entries_num; ++i)
    {
        entry = &plugins->entries[i];
        if (strcmp(plugin_name, entry->plugin_name) == 0)
        {
            return entry;
        }
    }

    return NULL;
}

// 让plugin对自己的setting进行特殊处理

// 把master加载的任务设置，分协议加载，转换成自己的类型
// 直接对settings中的void*进行修改，原来是JsonObj，改为自己的类型
int plugins_load_task_setting(ls_task_setting_t* settings, ls_plugin_t* plugins) {
    printf("==== plugins_load_task_setting\n");

    // 遍历settings，调用task_init，把返回的setting保存在plugin中
    for (ls_task_setting_t::iterator it = settings->begin(); it != settings->end(); ++it)
    {
        string plugin_name = it->first;
        printf("  plugin=%s\n", plugin_name.c_str());
        // 根据plugin_name找到plugin_entry，进一步找到task_init回调
        ls_plugin_entry_t* entry = find_entry_by_name(plugin_name.c_str(), plugins);
        if (entry == NULL)
        {
            printf("  no plugin [%s] loaded\n", plugin_name.c_str());
            return -1;
        }
        
        // 更新plugin_setting，初始化plugin_state
        if ((entry->task_init)(it->second) < 0)
        {
            printf("ERROR task_init error\n");
            return -1;
        }
    }
    return 0;
}

int plugins_unload_task_setting(ls_plugin_t* plugins) {
    printf("==== plugins_unload_task_setting()\n");

    ls_plugin_entry_t* entry;
    for (size_t i = 0; i < plugins->entries_num; ++i)
    {
        entry = &plugins->entries[i];
        if ((entry->task_destroy)() < 0) {
            printf("ERROR failed to task_destroy()\n");
            return -1;
        }
    }

    return 0;
}

ls_plugin_api_entry_t* find_api_entry_by_name(const char* name, const ls_plugin_api_t* apis) {
    ls_plugin_api_entry_t* entry;
    for (size_t i = 0; i < apis->entries_num; ++i)
    {
        entry = &apis->entries[i];
        if (strcmp(name, entry->name) == 0)
        {
            return entry;
        }
    }

    return NULL;
}

// 让plugin对script进行特殊处理。例如明确哪个API是属于自己的，设置好ls_api_t
int plugins_load_task_script(ls_task_script_t* script, ls_plugin_t* plugins) {
    printf("==== plugins_load_task_script\n");

    ls_task_script_entry_t* script_entry;    for (size_t i = 0; i < script->entries_num; ++i)
    {
        // find plugin_name of script_entry in plugins
        script_entry = script->entries + i;
        ls_plugin_entry_t* entry = find_entry_by_name(script_entry->plugin_name.c_str(), plugins);
        if (entry == NULL)
        {
            printf("  plugin_name %s not found\n", script_entry->plugin_name.c_str());
            return -1;
        }

        // set api of script_entry in plugins
        ls_plugin_api_entry_t* api_entry = find_api_entry_by_name(script_entry->api_name.c_str(), &entry->apis);
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

int plugins_unload_task_script(ls_task_script_t* script, ls_plugin_t* plugins) {
    printf("==== plugins_unload_task_script()\n");

    ls_plugin_entry_t* entry;
    ls_task_script_entry_t* script_entry;
    for (size_t i = 0; i < script->entries_num; ++i)
    {
        script_entry = script->entries + i;
        // find plugin_name of script_entry in plugins
        entry = find_entry_by_name(script_entry->plugin_name.c_str(), plugins);
        if (entry == NULL)
        {
            printf("  plugin_name %s not found\n", script_entry->plugin_name.c_str());
            return -1;
        }

        ls_plugin_api_entry_t* api_entry = find_api_entry_by_name(script_entry->api_name.c_str(), &entry->apis);
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
