#include <stdio.h>

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
    printf("====start_workers()\n");
    int workers_num = master->config.worker_num;

    for (int i = 0; i < workers_num; ++i)
    {
        ls_worker_t* w = new ls_worker_t();

        master->workers.push_back(w);

        if (uv_async_init(master->master_loop, &(w->master_async), master_async_callback) < 0) {
            return -1;/* TODO */
        }

        if (init_worker(w) < 0)
        {
            return -1;/* TODO */
        }
    }

    return 0;
}

int reap_workers(ls_master_t* master) {
    return -1;
}

static int notify_worker_start_new_session(ls_worker_t* w, int num) {
    int* num2 = new int;
    *num2 = num;
    w->worker_async.data = num2;
    return uv_async_send(&(w->worker_async));
}

int start_new_session(int num) {
    printf("==== start_new_session(%d)\n", num);
    // 先按简单的方式来：num尽量平均分给每个worker，不考虑worker当前的会话数
    int worker_num = master.config.worker_num;
    int avg = num/worker_num + 1;
    int add = num%worker_num;
    printf("  avg=%d,add=%d\n", avg, add);

    for (int i = 0; i < add; ++i)
    {
        if (notify_worker_start_new_session(master.workers[i], avg) < 0)
        {
            printf("  Failed to worker_start_new_session()\n");
            return -1;
        }
    }

    for (int i = add; i < worker_num; ++i)
    {
        if (notify_worker_start_new_session(master.workers[i], avg-1) < 0)
        {
            printf("  Failed to worker_start_new_session()\n");
            return -1;
        }
    }

    return 0;
}
