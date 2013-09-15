
#include "ls_worker.h"

void do_worker(void* arg) {
    ls_worker_t* worker = arg;
    worker->worker_loop = uv_default_loop();
    uv_async_init(worker->worker_loop, &(worker->master_to_worker), async_todo);

    xxxx;// TODO
    
    uv_run(worker->worker_loop, UV_RUN_DEFAULT);
}

int init_worker(ls_worker_t* w) {
    w->worker_loop = uv_new_loop();
    uv_async_init(w->worker_loop, &(workers[i].master_async), async_todo);
    uv_thread_create(&(w->thread), do_worker, args_todo);

    return 0;
}

