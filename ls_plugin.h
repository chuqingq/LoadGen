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
#include "ls_stats.h"

struct ls_plugin_entry_s;

typedef int (*ls_plugin_declare_t)(struct ls_plugin_entry_s* plugin_entry);

typedef int (*ls_plugin_load_t)();
typedef int (*ls_plugin_unload_t)();
typedef int (*ls_plugin_task_init_t)(const Json::Value* setting);
typedef int (*ls_plugin_task_destroy_t)();
typedef int (*ls_plugin_session_init_t)(void** state);
typedef int (*ls_plugin_session_destroy_t)(void** state);

typedef int (*ls_plugin_api_init_t)(const Json::Value* json_args, void** args);
typedef int (*ls_plugin_api_destroy_t)(void** args);
typedef int (*ls_plugin_api_run_t)(const void* args, ls_session_t* session, map<string, string> * vars);

typedef struct {
    char* name;
    ls_plugin_api_init_t init;
    ls_plugin_api_run_t run;
    ls_plugin_api_destroy_t destroy;
} ls_plugin_api_entry_t;

typedef struct ls_plugin_entry_s {
    // 1.plugin_name
    char* plugin_name;

    // 2.callbacks
    ls_plugin_load_t plugin_load;// master启动时调用
    ls_plugin_unload_t plugin_unload;// master退出前调用

    ls_plugin_task_init_t task_init;// master在任务启动前调用。加载setting，脚本参数预处理
    ls_plugin_task_destroy_t task_destroy;// master在任务结束后调用。卸载setting

    ls_plugin_session_init_t session_init;
    ls_plugin_session_destroy_t session_destroy;

    // 3.APIs
    ls_plugin_api_entry_t* apis;
    size_t num_apis;

    // 4.stats
    ls_stats_entry_t* stats;
    size_t stats_num;

    // private
    uv_lib_t plugin_lib;
    ls_plugin_declare_t plugin_declare;
} ls_plugin_entry_t;

typedef struct {
    ls_plugin_entry_t* entries;
    size_t entries_num;
} ls_plugin_t;

int load_plugins(ls_plugin_t* plugins);
int unload_plugins(ls_plugin_t* plugins);

int plugins_load_task_setting(ls_task_setting_t* settings, ls_plugin_t* plugins);
int plugins_unload_task_setting(ls_plugin_t* plugins);

int plugins_load_task_script(ls_task_script_t* script, ls_plugin_t* plugins);
int plugins_unload_task_script(ls_task_script_t* script, ls_plugin_t* plugins);

#endif
