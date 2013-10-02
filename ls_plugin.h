#ifndef LS_PLUGIN_H_
#define LS_PLUGIN_H_

#include <string>
#include <map>
using namespace std;

#include "lib/libuv/include/uv.h"
#include "jsoncpp/json/json.h"

#include "ls_worker.h"
#include "ls_task_setting.h"
#include "ls_task_script.h"

struct ls_plugin_entry_s;

typedef int (*ls_plugin_declare_t)(const char** plugin_name,
                                   struct ls_plugin_entry_s* plugin_entry);

typedef int (*ls_plugin_load_t)();
typedef int (*ls_plugin_unload_t)();

typedef int (*ls_plugin_task_init_t)(const Json::Value* setting,
                                     void** plugin_state);
typedef int (*ls_plugin_task_destroy_t)(void** plugin_setting,
                                        void** plugin_state);

typedef int (*ls_plugin_api_prepare_t)(const Json::Value* json_args,
                                       void** args);

typedef int (*ls_plugin_api_t)(const void* args,
                               ls_session_t* session,
                               /* void* plugin_state,*/
                               map<string, string> * vars);

typedef struct {
    ls_plugin_api_prepare_t prepare;
    ls_plugin_api_t api;
} ls_plugin_api_entry_t;


typedef struct ls_plugin_entry_s {
    ls_plugin_load_t plugin_load;// master启动时调用
    ls_plugin_unload_t plugin_unload;// master退出前调用

    ls_plugin_task_init_t task_init;// master在任务启动前调用。加载setting，脚本参数预处理？？ TODO
    ls_plugin_task_destroy_t task_destroy;// master在任务结束后调用。卸载setting
    
    map<string, ls_plugin_api_entry_t> apis;// TODO 一个api包含两项，第一个prepare，第二个是api

    void* plugin_setting;// 只读，在task_init中来自task_setting
    void* plugin_state;// TODO 需要动态变化，其实不需要存在plugin_entry中

    // private
    uv_lib_t plugin_lib;
    ls_plugin_declare_t plugin_declare;
} ls_plugin_entry_t;

// plugin_name -> ls_plugin_entry_t
typedef map<string, ls_plugin_entry_t> ls_plugin_t;

int load_plugins(ls_plugin_t* plugins);
int unload_plugins(ls_plugin_t* plugins);

int plugins_load_task_setting(ls_task_setting_t* settings, ls_plugin_t* plugins);
int plugins_unload_task_setting(ls_plugin_t* plugins);

int plugins_load_task_script(ls_task_script_t* script, ls_plugin_t* plugins);
int plugins_unload_task_script(ls_plugin_t* plugins);

#endif
