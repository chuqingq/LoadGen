#ifndef LS_PLUGIN_H_
#define LS_PLUGIN_H_

#include <string>
#include <map>
using namespace std;

#include "lib/libuv/include/uv.h"

#include "ls_task_setting.h"

struct ls_plugin_entry_s;

typedef int (*ls_plugin_declare_t)(const char** plugin_name, struct ls_plugin_entry_s* plugin_entry);

typedef int (*ls_plugin_load_t)();
typedef int (*ls_plugin_unload_t)();

typedef int (*ls_plugin_task_init_t)(const void* setting, void** plugin_setting, void** plugin_state);
typedef int (*ls_plugin_task_destroy_t)(void** plugin_setting, void** plugin_state);

typedef int (*ls_plugin_api_t)(const void* args, void* plugin_state, map<string, string> * vars);


typedef struct ls_plugin_entry_s {
    uv_lib_t plugin_lib;
    ls_plugin_declare_t plugin_declare;

    ls_plugin_load_t plugin_load;// master启动时调用
    ls_plugin_unload_t plugin_unload;// master退出前调用

    ls_plugin_task_init_t task_init;// master在任务启动前调用。加载setting，脚本参数预处理？？ TODO
    ls_plugin_task_destroy_t task_destroy;// master在任务结束后调用。卸载setting
    
    map<string, ls_plugin_api_t> apis;

} ls_plugin_entry_t;

// ls_plugin_t
typedef map<string/* plugin_name */, ls_plugin_entry_t> ls_plugin_t;

int load_plugins(ls_plugin_t* plugins);

int unload_plugins(ls_plugin_t* plugins);

// 把master加载的任务设置，分协议加载，转换成自己的类型
// 直接对settings中的void*进行修改，原来是JsonObj，改为自己的类型
int plugins_load_task_setting(ls_task_setting_t* settings,
                              ls_plugin_t* plugins);

int plugins_unload_task_setting(ls_plugin_t* plugins);

#endif
