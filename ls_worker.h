#ifndef LS_WORKER_H_
#define LS_WORKER_H_

#include <map>
using namespace std;

#include "libuv/include/uv.h"

#include "ls_task_script.h"
#include "ls_task_setting.h"
#include "ls_task_var.h"
#include "ls_session.h"


typedef struct ls_worker_s {
    uv_thread_t thread;
    uv_loop_t* worker_loop;

    vector<ls_session_t*>* sessions;// TODO reuse. use QUEUE

    /* private */
    int worker_started;// specify worker already started

    // worker和master的通信
    uv_async_t master_async;
    uv_async_t worker_async;
    int callmodel_delta;// 目前-1表示worker停止，其他值表示worker启动/停止相应个session TODO 0:stop
    uv_rwlock_t callmodel_delta_lock;
} ls_worker_t;

int init_worker(ls_worker_t* w);// create thread
int reap_worker(ls_worker_t* w);// join thread
// int notify_worker(const string& cmd);// to delete

int worker_start_new_session(ls_worker_t* w, int num);

// async callbacks
int worker_do_callmodel(ls_worker_t* w);// TODO return value
int worker_stop(ls_worker_t* w);

int worker_set_callmodel_delta(ls_worker_t* w, int delta);// TODO ??
int worker_get_callmodel_delta(ls_worker_t* w, int* delta);

#endif
