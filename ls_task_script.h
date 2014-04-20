#pragma once

#include <string>
#include <vector>
using namespace std;

// #include "jsoncpp/json/json.h"
#include "lib/libjson/include/libjson.h"

#include "ls_plugin.h"

struct ls_plugin_api_s;

typedef struct {
    string api_name;
    string plugin_name;// Json::Value.asString() return a temp obj.

    // Json::Value json_args;
    JSONNODE* json_args;

    // content parsed in plugins_load_task_script()
    struct ls_plugin_api_s* api;// api_run
    void* args;// api_init()中分配，api_destroy()中释放
} ls_task_script_entry_t;

typedef struct ls_task_script_s {
    // Json::Value script;
    JSONNODE* script;

    ls_task_script_entry_t* entries;
    size_t entries_num;
} ls_task_script_t;

int load_task_script(ls_task_script_t* script);
int unload_task_script(ls_task_script_t* script);
