#include <stdio.h>

#include "ls_utils.h"
#include "ls_master.h"
#include "ls_plugin.h"

int load_plugins(ls_plugin_t* plugins) {
    // 加载plugin目录下的插件
    ls_master_t* master = container_of(plugins, ls_master_t, plugins);
    const vector<string> &plugin_paths = master->config.plugin_paths;

    for (int i = 0; i < plugin_paths.size(); ++i)
    {
        ls_plugin_entry_t entry;
        if (uv_dlopen(plugin_paths[i].c_str(), &(entry.plugin_lib)) < 0) {
            printf("Failed to uv_dlopen\n");
            return -1;
        }

        if (uv_dlsym(&(entry.plugin_lib), "plugin_declare", (void**)&(entry.plugin_declare)) < 0) {
            printf("Failed to uv_dlsym\n");
            return -1;
        }

        const char* plugin_name = NULL;
        if ((entry.plugin_declare(&plugin_name, &entry) < 0)) {
            printf("Failed to plugin_declare\n");
            return -1;
        }

        plugins->insert(pair<string, ls_plugin_entry_t>(plugin_name, entry));
    }

    return 0;
}

int unload_plugins(ls_plugin_t* plugins) {// TODO
    return -1;
}

// 把master加载的任务设置，分协议加载，转换成自己的类型
// 直接对settings中的void*进行修改，原来是JsonObj，改为自己的类型
int plugins_load_task_setting(ls_task_setting_t* settings,
                              ls_plugin_t* plugins) {
    return -1;
}

int plugins_unload_task_setting(ls_plugin_t* plugins) {// TODO
    return -1;
}
