#include <stdio.h>

#include "ls_utils.h"
#include "ls_master.h"
#include "ls_plugin.h"

int load_plugins(ls_plugin_t* plugins) {
    printf("==== load_plugins()\n");

    // 加载plugin目录下的插件
    ls_master_t* master = container_of(plugins, ls_master_t, plugins);
    const vector<string> &plugin_paths = master->config.plugin_paths;

    for (int i = 0; i < plugin_paths.size(); ++i)
    {
        ls_plugin_entry_t entry;
        if (uv_dlopen(plugin_paths[i].c_str(), &(entry.plugin_lib)) < 0) {
            printf("  Failed to uv_dlopen\n");
            return -1;
        }

        if (uv_dlsym(&(entry.plugin_lib), "plugin_declare", (void**)&(entry.plugin_declare)) < 0) {
            printf("  Failed to uv_dlsym\n");
            return -1;
        }

        const char* plugin_name = NULL;
        if ((entry.plugin_declare(&plugin_name, &entry) < 0)) {
            printf("  Failed to plugin_declare\n");
            return -1;
        }

        printf("  load_plugins: %s task_init=%d\n", plugin_name, entry.task_init);

        plugins->insert(pair<string, ls_plugin_entry_t>(plugin_name, entry));

        if ((entry.plugin_load)() < 0)
        {
            return -1;
        }
    }

    return 0;
}

int unload_plugins(ls_plugin_t* plugins) {// TODO
    return -1;
}

// 让plugin对自己的setting进行特殊处理

// 把master加载的任务设置，分协议加载，转换成自己的类型
// 直接对settings中的void*进行修改，原来是JsonObj，改为自己的类型
int plugins_load_task_setting(ls_task_setting_t* settings,
                              ls_plugin_t* plugins) {
    printf("==== plugins_load_task_setting\n");
    // 遍历settings，调用task_init，把返回的setting保存在plugin中
    for (ls_task_setting_t::iterator it = settings->begin(); it != settings->end(); ++it)
    {
        string plugin_name = it->first;
        printf("  load plugin %s:\n", plugin_name.c_str());
        void* plugin_setting = NULL;
        // 根据plugin_name找到plugin_entry，进一步找到task_init回调
        ls_plugin_entry_t* entry = &((*plugins)[plugin_name]);
        if (entry->task_init == NULL)
        {
            printf("  no plugin [%s] loaded\n", plugin_name.c_str());
            return -1;
        }
        printf("  ls_plugin_entry: %d\n", entry);
        printf("  after ls_plugin_entry: %d\n", entry->task_init);
        // 更新plugin_setting，初始化plugin_state
        if ((entry->task_init)(it->second, &plugin_setting, &(entry->plugin_state)) < 0)
        {
            printf("ERROR task_init error\n");
            return -1;
        }
        printf("  plugin [%s] task_init successfully\n", plugin_name.c_str());

        entry->plugin_setting = plugin_setting;
    }
    return 0;
}

int plugins_unload_task_setting(ls_plugin_t* plugins) {// TODO
    return -1;
}

// TODO 让plugin对script进行特殊处理。例如明确哪个API是属于自己的，设置好ls_api_t
int plugins_load_task_script(ls_task_script_t* script, ls_plugin_t* plugins) {
    printf("====plugins_load_task_script\n");

    for (ls_task_script_t::iterator it = script->begin(); it != script->end(); ++it)
    {
        // find plugin_name of script_entry in plugins
        ls_plugin_t::iterator plugins_it = plugins->find(it->plugin_name);
        if (plugins_it == plugins->end())
        {
            printf("  plugin_name %s not found\n", it->plugin_name.c_str());
            return -1;
        }

        // set api of script_entry in plugins
        map<string, ls_plugin_api_entry_t>::iterator api_it = plugins_it->second.apis.find(it->api_name);
        if (api_it == plugins_it->second.apis.end())
        {
            printf("  api %s not found\n", it->api_name.c_str());
            return -1;
        }

        void* args = NULL;
        if ((api_it->second.prepare)(&(it->json_args), &args) < 0)
        {
            printf("  api %s prepare error\n", it->api_name.c_str());
            return -1;
        }

        // 设置api和args
        it->api = (void*)api_it->second.api;
        it->args = args;
    }

    return 0;
}
