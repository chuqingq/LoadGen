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

    // ==================== private
    volatile int worker_started;// TODO private specify worker already started

    // worker和master的通信
    uv_async_t master_async;
    uv_async_t worker_async;
    int callmodel_delta;// 目前-1表示worker停止，其他值表示worker启动/停止相应个session TODO 0:stop
    uv_rwlock_t callmodel_delta_lock;
} ls_worker_t;

int worker_start(ls_worker_t* w);// create thread
int worker_stop(ls_worker_t* w);// finish all sessions and stop eventloop
int worker_reap(ls_worker_t* w);// join thread

int worker_do_callmodel(ls_worker_t* w);// async callbacks

int worker_set_callmodel_delta(ls_worker_t* w, int delta);
int worker_get_callmodel_delta(ls_worker_t* w, int* delta);

#endif
