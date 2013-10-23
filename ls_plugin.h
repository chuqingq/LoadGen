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

struct ls_master_s;
struct ls_worker_s;
struct ls_session_s;

struct ls_plugin_entry_s;

// typedef int (*ls_plugin_declare_t)(struct ls_plugin_entry_s* plugin_entry);

// typedef int (*ls_plugin_load_t)();
// typedef int (*ls_plugin_unload_t)();
// typedef int (*ls_plugin_task_init_t)(const Json::Value* setting);
// typedef int (*ls_plugin_task_terminate_t)();
// typedef int (*ls_plugin_session_init_t)(void** state);
// typedef int (*ls_plugin_session_terminate_t)(void** state);

// typedef int (*ls_plugin_api_init_t)(const Json::Value* json_args, void** args);
// typedef int (*ls_plugin_api_terminate_t)(void** args);
typedef int (*ls_plugin_api_run_t)(const void* args, ls_session_t* session, map<string, string> * vars);
// TODO

typedef struct {
    char* name;
    // ls_plugin_api_init_t init;
    // ls_plugin_api_run_t run;
    // ls_plugin_api_terminate_t terminate;
    int (*init)(const Json::Value* json_args, void** args);
    int (*run)(const void* args, ls_session_t* session, map<string, string> * vars);
    int (*terminate)(void** args);
} ls_plugin_api_t;

typedef struct ls_plugin_entry_s {
    size_t plugin_index;// TODO
    // 1.plugin_name
    char* plugin_name;

    // 2.callbacks
    // ls_plugin_load_t plugin_load;// master启动时调用
    // ls_plugin_unload_t plugin_unload;// master退出前调用
    int (*master_init)(struct ls_master_s* master);// TODO
    int (*master_terminate)(struct ls_master_s* master);

    int (*worker_init)(struct ls_worker_s* worker);// TODO
    int (*worker_terminate)(struct ls_worker_s* worker);

    // ls_plugin_task_init_t task_init;// master在任务启动前调用。加载setting，脚本参数预处理
    // ls_plugin_task_terminate_t task_terminate;// master在任务结束后调用。卸载setting
    int (*task_init)(ls_task_setting_t* script);// TODO
    int (*task_terminate)();// TODO lack ls_task_setting_t parameter

    // ls_plugin_session_init_t session_init;
    // ls_plugin_session_terminate_t session_terminate;
    int (*session_init)(ls_session_t* session);
    int (*session_terminate)(ls_session_t* session);

    // 3.APIs
    ls_plugin_api_t* apis;
    size_t num_apis;

    // 4.stats
    ls_stats_entry_t* stats;
    size_t stats_num;

    // private
    uv_lib_t plugin_lib;
    // ls_plugin_declare_t plugin_declare;
    int (*plugin_declare)(struct ls_plugin_entry_s* plugin_entry);
} ls_plugin_t;

// int load_plugins();
// int unload_plugins();

int plugins_load_task_setting(ls_task_setting_t* settings, ls_plugin_t* plugins, size_t num_plugins);
int plugins_unload_task_setting(ls_plugin_t* plugins, size_t num_plugins);

int plugins_load_task_script(ls_task_script_t* script, ls_plugin_t* plugins, size_t num_plugins);
int plugins_unload_task_script(ls_task_script_t* script, ls_plugin_t* plugins, size_t num_plugins);

#endif
