#pragma once

#include <string>
#include <vector>
using namespace std;

#include "lib/libjson/include/libjson.h"

#include "ls_plugin.h"

struct ls_plugin_api_s;

typedef struct ls_task_script_entry_s {
    // ls_plugin_entry_t* plugin;// 保存api所属的plugin_index
    struct ls_plugin_api_s* api;// TODO 解析脚本时直接设置，避免保存无用的api_name和plugin_name
    void* args;// api_init()中分配，api_destroy()中释放
} ls_task_script_entry_t;

typedef struct ls_task_script_s {
    ls_task_script_entry_t* entries;
    size_t entries_num;
} ls_task_script_t;

int load_task_script(ls_task_script_t* script);
int unload_task_script(ls_task_script_t* script);
