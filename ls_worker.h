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
    int worker_started;
    uv_thread_t thread;
    uv_loop_t* worker_loop;

    uv_async_t master_async;
    uv_async_t worker_async;
    int callmodel_delta;
    uv_rwlock_t callmodel_delta_lock;
    int a;
    int b;// TODO

    vector<ls_session_t*>* sessions;
    int c;
    int next_session_id;

} ls_worker_t;

int init_worker(ls_worker_t* w);
int reap_worker(ls_worker_t* w);
int notify_worker(const string& cmd);


int worker_start_new_session(ls_worker_t* w, int num);

int worker_set_callmodel_delta(ls_worker_t* w, int delta);
int worker_get_callmodel_delta(ls_worker_t* w, int* delta);

#endif
