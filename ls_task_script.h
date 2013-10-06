#ifndef LS_TASK_SCRIPT_H_
#define LS_TASK_SCRIPT_H_

#include <string>
#include <vector>
using namespace std;

#include "jsoncpp/json/json.h"

typedef struct {
    // content direct from 'task/script.json'
    string api_name;
    string plugin_name;
    Json::Value json_args;

    // content parsed in plugins_load_task_script()
    void* api;
    void* args;// api_init()中分配，api_destroy()中释放
} ls_task_script_entry_t;

typedef vector<ls_task_script_entry_t> ls_task_script_t;

int load_task_script(ls_task_script_t* script);

#endif
