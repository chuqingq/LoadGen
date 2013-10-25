#ifndef LS_PLUGIN_H_
#define LS_PLUGIN_H_

#include <string>
#include <map>
using namespace std;

#include "lib/libuv/include/uv.h"
#include "jsoncpp/json/json.h"

#include "ls_worker.h"
#include "ls_session.h"
#include "ls_task_setting.h"
#include "ls_task_script.h"
// #include "ls_stats.h"

struct ls_master_s;
struct ls_worker_s;
struct ls_session_s;
struct ls_task_script_s;
struct ls_plugin_entry_s;

typedef struct ls_plugin_api_s {
    char* name;

    int (*init)(const Json::Value* json_args, void** args);
    int (*run)(const void* args, ls_session_t* session, map<string, string> * vars);
    int (*terminate)(void** args);
} ls_plugin_api_t;

typedef struct ls_plugin_entry_s {
    // 1.plugin_id/name
    size_t plugin_index;
    char* plugin_name;

    // 2.callbacks
    int (*master_init)(struct ls_master_s* master);
    int (*master_terminate)(struct ls_master_s* master);

    int (*script_init)(ls_task_setting_t* setting);
    int (*script_terminate)(ls_task_setting_t* setting);

    int (*task_init)();
    int (*task_terminate)();

    int (*worker_init)(struct ls_worker_s* worker);
    int (*worker_terminate)(struct ls_worker_s* worker);

    int (*session_init)(struct ls_session_s* session);
    int (*session_terminate)(struct ls_session_s* session);

    // 3.APIs
    ls_plugin_api_t* apis;
    size_t num_apis;

    // 4.stats
    // ls_stats_entry_t* stats;
    // size_t stats_num;

    /* private */
    uv_lib_t plugin_lib;
    int (*plugin_declare)(struct ls_plugin_entry_s* plugin_entry);
} ls_plugin_t;

int plugins_script_init(ls_task_setting_t* settings, struct ls_task_script_s* script, ls_plugin_t* plugins, size_t num_plugins);
int plugins_script_terminate(ls_task_setting_t* settings, struct ls_task_script_s* script, ls_plugin_t* plugins, size_t num_plugins);

int plugins_task_init(ls_plugin_t* plugins, size_t num_plugins);
int plugins_task_terminate(ls_plugin_t* plugins, size_t num_plugins);

#endif
