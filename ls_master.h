#pragma once

#include "lib/libuv/include/uv.h"

#include "ls_worker.h"
#include "ls_plugin.h"
#include "ls_task_callmodel.h"
#include "ls_task_setting.h"
#include "ls_task_var.h"
#include "ls_task_script.h"

typedef struct ls_master_s {
    uv_loop_t* master_loop;

    ls_plugin_t* plugins;
    size_t num_plugins;

    ls_task_callmodel_t callmodel;
    ls_task_var_t vars;     // 变量，master读取，master不需要，worker只读
    ls_task_script_t script;// 脚本，master保存，worker只读
    
    ls_worker_t* workers;
    size_t num_workers;
} ls_master_t;

extern ls_master_t master;

int start_master(ls_master_t* master);
int stop_master(ls_master_t* master);

int start_workers(ls_master_t* master);// -> worker_start()
int stop_workers(ls_master_t* master);// -> worker_stop()
int reap_workers(ls_master_t* master);// -> worker_reap()

int start_new_session(int num);
