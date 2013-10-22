#ifndef LS_SESSION_H_
#define LS_SESSION_H_

#include <map>
using namespace std;

#include "ls_task_script.h"
#include "ls_task_var.h"
#include "libuv/include/uv.h"

struct ls_session_s;
typedef int (*ls_session_process_t)(struct ls_session_s*);

typedef struct ls_session_s {
    uv_loop_t* loop;// ref to worker's loop

    const ls_task_script_t* script;
    size_t script_cur;

    void** states;// states of plugins, one by one
    ls_task_var_t cur_vars;

    ls_session_process_t process;
    ls_session_process_t finish;
    // TODO report_stats()
} ls_session_t;


int process_session(ls_session_t* session);

int finish_session(ls_session_t* session);

#endif
