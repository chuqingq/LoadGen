#include <stdio.h>

#include "ls_utils.h"
#include "ls_worker.h"

static void worker_async_callback(uv_async_t* async, int status) {\
    printf("====worker_async_callback()\n");

    ls_worker_t* w = container_of(async, ls_worker_t, worker_async);
    // worker收到master的消息
    int delta = *((int*)async->data);
    printf("\tworker[%d] session_num delta=%d\n", w->thread, delta);

    // TODO 1. 增加/变化呼叫量
    //  读取script，按照逻辑操作
    // TODO 2. 停止：如何停止整个worker？？
    uv_loop_delete(w->worker_loop);// ???
}

static void worker_thread(void* arg) {
    ls_worker_t* w = (ls_worker_t*)arg;

    w->worker_loop = uv_loop_new();
    uv_async_init(w->worker_loop, &(w->worker_async), worker_async_callback);

    uv_run(w->worker_loop, UV_RUN_DEFAULT);
}

int init_worker(ls_worker_t* w) {
    printf("====init_worker()\n");
    // master_async在master中初始化
    return uv_thread_create(&(w->thread), worker_thread, (void*)w);
}

int reap_worker(ls_worker_t* w) {
    return uv_thread_join(&(w->thread));
}

// 在一个worker上启动num个会话
int worker_start_new_session(ls_worker_t* w, int num) {
    printf("====worker_start_new_session(%d, %d)\n", w->thread, num);
    // TODO
    return 0;
}
