#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>

#include "ls_master.h"
#include "ls_worker.h"

ls_master_t master;

static void master_async_callback(uv_async_t* handle, int status) {
    // TODO master接收worker发来的消息 statistics
    // 1. worker上报的统计信息：暂时直接打印
    // printf("master recv worker async msg\n");
    // 2. worker线程退出会如何？？ TODO
}

int start_workers(ls_master_t* master) {
    printf("==== start_workers()\n");

    int workers_num = master->config.workers_num;
    master->workers = (ls_worker_t*)malloc(workers_num * sizeof(ls_worker_t));
    if (master->workers == NULL)
    {
        printf("ERROR failed to malloc workers\n");
        return -1;
    }

    for (int i = 0; i < workers_num; ++i)
    {
        ls_worker_t* w = master->workers + i;
        w->worker_started = 0;

        if (uv_async_init(master->master_loop, &(w->master_async), master_async_callback) < 0) {
            printf("ERROR failed to uv_async_init(master)\n");
            return -1;
        }

        if (init_worker(w) < 0)// start worker thread
        {
            printf("ERROR failed to init_worker()\n");
            return -1;
        }

        while (w->worker_started == 0) ;
    }

    return 0;
}

int stop_workers(ls_master_t* master) {
    printf("==== stop_workers()\n");

    for (int i = 0; i < master->config.workers_num; ++i)
    {
        ls_worker_t* w = master->workers + i;

        if (worker_set_callmodel_delta(w, -1) < 0)
        {
            printf("ERROR failed to worker_set_callmodel_delta()\n");
            return -1;
        }

        if (uv_async_send(&(w->worker_async)) < 0)
        {
            printf("ERROR failed to uv_async_send()\n");
            return -1;
        }
    }

    return 0;
}

int reap_workers(ls_master_t* master) {
    printf("==== reap_workers()\n");

    for (int i = 0; i< master->config.workers_num; ++i)
    {
        printf("  before reap_workers(%d)\n", i);

        if (reap_worker(master->workers + i) < 0) {
            printf("ERROR failed to read_worker(%d)\n", i);
            return -1;
        }
    }

    return 0;
}

static int notify_worker_start_new_session(ls_worker_t* w, int num) {
    printf("  ==== notify_worker_start_new_session(%lu, %d)\n", (unsigned long)w, num);

    if (worker_set_callmodel_delta(w, num) < 0)
    {
        printf("ERROR failed to worker_set_callmodel_delta()\n");
        return -1;
    }

    return uv_async_send(&(w->worker_async));
}

int start_new_session(int num) {
    printf("  ==== start_new_session(%d)\n", num);
    
    // 先按简单的方式来：num尽量平均分给每个worker，不考虑worker当前的会话数
    int workers_num = master.config.workers_num;
    int avg = num/workers_num + 1;
    int add = num%workers_num;
    printf("  avg=%d,add=%d\n", avg, add);

    for (int i = 0; i < add; ++i)
    {
        if (notify_worker_start_new_session(master.workers + i, avg) < 0)
        {
            printf("  Failed to worker_start_new_session()\n");
            return -1;
        }
    }

    if ((avg - 1) == 0)
    {
        return 0;
    }

    for (int i = add; i < workers_num; ++i)
    {
        if (notify_worker_start_new_session(master.workers + i, avg-1) < 0)
        {
            printf("  Failed to worker_start_new_session()\n");
            return -1;
        }
    }

    return 0;
}
