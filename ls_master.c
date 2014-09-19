#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>

#include "ls_utils.h"
#include "ls_master.h"
#include "ls_worker.h"

ls_master_t master;


static void master_async_callback(uv_async_t* handle, int status) {
    LOG("  master_async_callback()\n");
}

int start_master(ls_master_t* master) {
    ls_plugin_t* p;
    for (size_t i = 0; i < master->num_plugins; ++i) {
        p = master->plugins + i;
        if (p->master_init != NULL && (p->master_init)(master) < 0) {
            LOGE("ERROR failed to master_init()\n");
            return -1;
        }
    }
    return 0;
}

int stop_master(ls_master_t* master) {
    ls_plugin_t* p;
    for (size_t i = 0; i < master->num_plugins; ++i) {
        p = master->plugins + i;
        if (p->master_terminate != NULL && (p->master_terminate)(master) < 0) {
            LOGE("ERROR failed to master_terminate()\n");
            // return -1;
        }
    }
    return 0;
}

int start_workers(ls_master_t* master) {
    LOG("start_workers()\n");

    master->num_workers = master->config.workers_num;
    master->workers = (ls_worker_t*)malloc(master->num_workers * sizeof(ls_worker_t));
    if (master->workers == NULL)
    {
        LOG("ERROR failed to malloc workers\n");
        return -1;
    }

    for (size_t i = 0; i < master->num_workers; ++i)
    {
        ls_worker_t* w = master->workers + i;
        w->worker_started = 0;

        if (uv_async_init(master->master_loop, &(w->master_async), master_async_callback) < 0) {
            LOG("ERROR failed to uv_async_init(master)\n");
            return -1;
        }

        if (worker_start(w) < 0)// start worker thread
        {
            LOG("ERROR failed to worker_start()\n");
            return -1;
        }

        while (w->worker_started == 0) ;
    }

    return 0;
}

// 通知worker自己停止
int stop_workers(ls_master_t* master) {
    LOG("stop_workers()\n");

    for (size_t i = 0; i < master->num_workers; ++i)
    {
        ls_worker_t* w = master->workers + i;
        w->worker_async.data = (void*)worker_stop;

        if (uv_async_send(&w->worker_async) < 0)
        {
            LOG("ERROR failed to uv_async_send()\n");
            return -1;
        }
    }

    return 0;
}

int reap_workers(ls_master_t* master) {
    LOG("reap_workers()\n");

    for (size_t i = 0; i< master->num_workers; ++i)
    {
        if (worker_reap(master->workers + i) < 0) {
            LOG("ERROR failed to worker_reap(%zu)\n", i);
            return -1;
        }
    }

    return 0;
}

static int notify_worker_do_callmodel(ls_worker_t* w, int num) {
    LOG("  notify_worker_do_callmodel(%lu, %d)\n", (unsigned long)w, num);

    if (worker_set_callmodel_delta(w, num) < 0)
    {
        LOG("ERROR failed to worker_set_callmodel_delta()\n");
        return -1;
    }

    w->worker_async.data = (void*)worker_do_callmodel;
    return uv_async_send(&(w->worker_async));
}

int start_new_session(int num) {
    LOG("  start_new_session(%d)\n", num);
    
    // 先按简单的方式来：num尽量平均分给每个worker，不考虑worker当前的会话数
    int workers_num = master.num_workers;
    int avg = num/workers_num + 1;
    int add = num%workers_num;
    LOG("  avg=%d,add=%d\n", avg, add);

    for (int i = 0; i < add; ++i)
    {
        if (notify_worker_do_callmodel(master.workers + i, avg) < 0)
        {
            LOG("ERROR failed to worker_start_new_session()\n");
            return -1;
        }
    }

    if ((avg - 1) == 0)
    {
        return 0;
    }

    for (int i = add; i < workers_num; ++i)
    {
        if (notify_worker_do_callmodel(master.workers + i, avg-1) < 0)
        {
            LOG("ERROR failed to worker_start_new_session()\n");
            return -1;
        }
    }

    return 0;
}
