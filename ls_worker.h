#ifndef LS_WORKER_H_
#define LS_WORKER_H_

#include <map>
using namespace std;

#include "libuv/include/uv.h"

#include "ls_task_script.h"
#include "ls_task_setting.h"
#include "ls_task_var.h"
#include "ls_session.h"


typedef struct {
    uv_thread_t thread;
    uv_loop_t* worker_loop;

    uv_async_t master_async;
    uv_async_t worker_async;

    vector<ls_session_t*> sessions;

} ls_worker_t;

int init_worker(ls_worker_t* w);
int reap_worker(ls_worker_t* w);
int notify_worker(const string& cmd);


int worker_start_new_session(ls_worker_t* w, int num);


#endif
