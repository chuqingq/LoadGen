#include "ls_plugin.h"

int load_plugins(ls_plugin_t* plugins) {
    // 加载plugin目录下的插件
    return -1;
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
