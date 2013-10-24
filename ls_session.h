#ifndef LS_SESSION_H_
#define LS_SESSION_H_

#include <map>
using namespace std;

#include "ls_worker.h"
#include "ls_task_script.h"
#include "ls_task_var.h"
#include "libuv/include/uv.h"

struct ls_task_script_s;
struct ls_worker_s;

struct ls_session_s;
typedef int (*ls_session_process_t)(struct ls_session_s*);

typedef struct ls_session_s {
    struct ls_worker_s* worker;

    const struct ls_task_script_s* script;
    size_t script_cur;

    // void** states;// states of plugins, one by one
    void** plugin_states;// TODO delete stats above
    size_t num_plugin_states;

    ls_task_var_t cur_vars;

    ls_session_process_t process;
    ls_session_process_t finish;
    // TODO report_stats()
} ls_session_t;


int process_session(ls_session_t* session);

int finish_session(ls_session_t* session);

#endif
