#ifndef LS_MASTER_H_
#define LS_MASTER_H_

#include <string>
#include <map>
#include <vector>
using namespace std;

#include "libuv/include/uv.h"

#include "ls_worker.h"
#include "ls_config.h"
#include "ls_plugin.h"
#include "ls_task_callmodel.h"
#include "ls_task_setting.h"
#include "ls_task_var.h"
#include "ls_task_script.h"

typedef struct {
    uv_loop_t* master_loop;

    ls_config_t config;
    ls_plugin_t plugins;
    // ls_plugin_entry_t* plugins;// num = master.config.plugins_num // TODO to delete

    ls_task_callmodel_t callmodel;// 呼叫模型，master保存，master使用
    ls_task_setting_t settings;// proto_type -> task_setting master读取setting.json并调用plugin的plugin_load_setting解析，后续master不用，worker只读
    ls_task_var_t vars;// 变量，master读取，master不需要，worker只读
    ls_task_script_t script;// 脚本，master保存，worker只读
    
    vector<ls_worker_t*> workers;
} ls_master_t;

extern ls_master_t master;

int start_workers(ls_master_t* master);
int stop_workers(ls_master_t* master);
int reap_workers(ls_master_t* master);

int start_new_session(int num);

#endif
