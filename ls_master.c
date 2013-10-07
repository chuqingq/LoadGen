#include <stdio.h>

#include <unistd.h>

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
    printf("==== start_workers()\n");

    int workers_num = master->config.worker_num;

    for (int i = 0; i < workers_num; ++i)
    {
        ls_worker_t* w = new ls_worker_t();
        printf("  worker[i]=%lu\n", (unsigned long)w);

        master->workers.push_back(w);

        if (uv_async_init(master->master_loop, &(w->master_async), master_async_callback) < 0) {
            printf("ERROR failed to uv_async_init(master)\n");
            return -1;/* TODO */
        }

        if (init_worker(w) < 0)// start worker thread
        {
            printf("ERROR failed to init_worker()\n");
            return -1;
        }
    }

    sleep(3);// TODO make sure worker thread ready, so master can do_callmodel with worker_async

    return 0;
}

int stop_workers(ls_master_t* master) {
    printf("==== stop_workers()\n");

    for (size_t i = 0; i < master->workers.size(); ++i)
    {
        // uv_stop(master->workers[i]->worker_loop);
        // 不能直接用uv_stop这种方式，需要通过async发给worker，由它自己uv_stop
        ls_worker_t* w = master->workers[i];

        int* num = new int;
        *num = -1;
        w->worker_async.data = num;

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

    vector<ls_worker_t*>& workers = master->workers;
    for (size_t i = 0; i< workers.size(); ++i)
    {
        printf("  before reap_workers(%d)\n", i);
        if (reap_worker(workers[i]) < 0) {
            printf("ERROR failed to read_worker(%d)\n", i);
            return -1;
        }
    }

    return 0;
}

static int notify_worker_start_new_session(ls_worker_t* w, int num) {
    printf("==== notify_worker_start_new_session(%lu, %d)\n", (unsigned long)w, num);

    int* num2 = new int;
    *num2 = num;
    w->worker_async.data = (void*)num2;
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
        printf("  1.worker[%d]=%lu\n", i, (unsigned long)master.workers[i]);
        if (notify_worker_start_new_session(master.workers[i], avg) < 0)
        {
            printf("  Failed to worker_start_new_session()\n");
            return -1;
        }
    }

    for (int i = add; i < worker_num; ++i)
    {
        printf("  2.worker[%d]=%lu\n", i, (unsigned long)master.workers[i]);
        if (notify_worker_start_new_session(master.workers[i], avg-1) < 0)
        {
            printf("  Failed to worker_start_new_session()\n");
            return -1;
        }
    }

    return 0;
}
