#ifndef LS_TASK_SCRIPT_H_
#define LS_TASK_SCRIPT_H_

#include <string>
#include <vector>
using namespace std;

#include "jsoncpp/json/json.h"

typedef struct {
    string api_name;
    string plugin_name;// Json::Value.asString() return a temp obj.

    Json::Value json_args;

    // content parsed in plugins_load_task_script()
    void* api;// api_run
    void* args;// api_init()中分配，api_destroy()中释放
} ls_task_script_entry_t;

typedef struct {
    Json::Value script;

    ls_task_script_entry_t* entries;
    size_t entries_num;
} ls_task_script_t;

int load_task_script(ls_task_script_t* script);
int unload_task_script(ls_task_script_t* script);

#endif
