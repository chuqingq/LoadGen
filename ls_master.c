#include "ls_master.h"
#include "ls_worker.h"

ls_master_t master;

static void master_async_callback(uv_async_t* handle, int status) {
    // TODO master接收worker发来的消息
    // 1. worker上报的统计信息：暂时直接打印
    // printf("master recv worker async msg\n");
    // 2. 线程退出会如何？？ TODO
}

int start_workers(ls_master_t* master) {
    int workers_num = master->config.worker_num;

    for (int i = 0; i < workers_num; ++i)
    {
        ls_worker_t w;

        if (uv_async_init(master->master_loop, &(w.master_async), master_async_callback) < 0) {
            return -1;/* TODO */
        }

        // if (start_worker(&w) < 0)
        // {
        //     return -1;/* TODO */
        // }
    }

    return 0;
}

int reap_workers(ls_master_t* master) {
    return -1;
}
