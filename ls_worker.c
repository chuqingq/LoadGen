
#include "ls_worker.h"

static void worker_async_callback(uv_async_t* handle, int status) {
    // worker收到master的消息
    // TODO 1. 增加/变化呼叫量
    //  读取script，按照逻辑操作
    // TODO 2. 停止：如何停止整个worker？？
    uv_loop_delete(w->worker_loop);// ???
}

static void worker_thread(void* arg) {
    ls_worker_t* w = arg;

    w->worker_loop = uv_loop_new();
    uv_async_init(w->worker_loop, &(w->worker_async), worker_async_callback);

    uv_run(w->worker_loop, UV_RUN_DEFAULT);
}

int init_worker(ls_worker_t* w) {
    // master_async在master中初始化
    return uv_thread_create(&(w->thread), worker_thread, (void*)w);
}

int reap_worker(ls_worker_t* w) {
    return uv_thread_join(&(w->thread));
}
