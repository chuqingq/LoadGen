#pragma once

#include <string>
#include <map>
using namespace std;

#include "lib/libuv/include/uv.h"
#include "lib/libjson/include/libjson.h"

#include "ls_worker.h"
#include "ls_session.h"
#include "ls_task_setting.h"
#include "ls_task_script.h"

struct ls_master_s;
struct ls_worker_s;
struct ls_session_s;
struct ls_task_script_s;
struct ls_plugin_entry_s;

typedef struct ls_plugin_api_s {
    struct ls_plugin_entry_s* plugin;

    const char* name;
    int (*init)(const JSONNODE* json_args, void** args);
    int (*run)(const void* args, void* sessionstate, map<string, string> * vars); // TODO vars是map吗？
    int (*terminate)(void** args);
} ls_plugin_api_t;

typedef struct ls_plugin_entry_s {
    // 1.plugin_id/name
    size_t plugin_index;
    const char* plugin_name;

    // 2.callbacks
    int (*plugin_init)(const JSONNODE* setting);// setting由plugin自己保存
    int (*plugin_terminate)();
    
    int (*master_init)(struct ls_master_s* master);// master_xxx/script_xxx/task_xxx
    int (*master_terminate)(struct ls_master_s* master);

    int (*worker_init)(struct ls_worker_s* worker);
    int (*worker_terminate)(struct ls_worker_s* worker);

    int (*session_init)(struct ls_session_s* session, void** sessionstate);
    int (*session_iterate)(void* sessionstate);
    int (*session_terminate)(void* sessionstate);

    // 3.APIs
    ls_plugin_api_t* apis;
    size_t num_apis;

    /* private */
    uv_lib_t plugin_lib;
    int (*plugin_declare)(struct ls_plugin_entry_s* plugin_entry);
} ls_plugin_t;

ls_plugin_api_t* find_api_by_name(const char* name, ls_plugin_t* plugins, size_t num_plugins);
