#ifndef LS_MASTER_H_
#define LS_MASTER_H_

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

    ls_plugin_t* plugins;
    size_t num_plugins;

    ls_task_callmodel_t callmodel;
    ls_task_setting_t settings;
    ls_task_var_t vars;// 变量，master读取，master不需要，worker只读
    ls_task_script_t script;// 脚本，master保存，worker只读
    
    ls_worker_t* workers;

    uv_timer_t stats_timer;
} ls_master_t;

extern ls_master_t master;

int start_workers(ls_master_t* master);
int stop_workers(ls_master_t* master);
int reap_workers(ls_master_t* master);

int start_new_session(int num);

// TODO
typedef enum ls_master_notify_type {
    NOTIFY_STATS = 0,
    NOTIFY_OTHER
} ls_master_notify_type;

int notify_master();
int do_notify();

int start_stats();

#endif
