#pragma once

#include "ls_worker.h"
#include "ls_task_script.h"
#include "ls_task_var.h"

struct ls_worker_s;

// struct ls_session_s;
// typedef int (*ls_session_process_t)(struct ls_session_s*);

// TODO 内存很敏感
typedef struct ls_session_s {
    struct ls_worker_s* worker;

    // const struct ls_task_script_s* script;
    size_t script_cur;

    void** plugin_states;
    // size_t num_plugin_states;// 个数和master中plugins中的个数相同

    ls_task_var_t cur_vars;
} ls_session_t;


int process_session(ls_session_t* session);// handle_session() -> api.run()

int finish_session(ls_session_t* session);// session_terminate(), free(plugin_states)
