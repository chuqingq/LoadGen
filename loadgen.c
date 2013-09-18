#include "ls_config.h"
#include "ls_script.h"
#include "ls_callmodel.h"
#include "ls_worker.h"

#include "uv.h"

uv_loop_t* master_loop;

ls_worker_t* workers;
int workers_num;

ls_config_t config;

static void master_async_callback(uv_async_t* handle, int status) {
    // TODO master接收worker发来的消息
    // 1. worker上报的统计信息：暂时直接打印
    printf("master recv worker async msg\n");
    // 2. 线程退出会如何？？ TODO
}

int init_workers() {
    workers_num = 2;/* TODO */
    workers = malloc(workers_num * sizeof(*workers));

    for (int i = 0; i < num_of_cpu; ++i)
    {
        ls_worker_t* w = workers + i;

        if (uv_async_init(master_loop, w->master_async, master_async_callback) < 0) {
            return -1;/* TODO */
        }

        if (init_worker(w) < 0)
        {
            return -1;/* TODO */
        }
    }

    return 0;
}

void reap_workers(ls_worker_t* w, int num) {
    for (int i = 0; i < num; ++i)
    {
        reap_worker(w + i);
    }
}


int main() {
    ls_script_t script;
    ls_callmodel_t callmodel;

    master_loop = uv_default_loop();

    if (read_config(&config) < 0) {
        return -1;
    }

    if (read_script(&script) < 0) {
        return -1;
    }

    if (read_callmodel(&callmodel) < 0) {
        return -1;
    }

    if (init_workers() < 0) {// 启动worker线程
        return -1;
    }

    if (do_callmodel(CallModel* callmodel) < 0) {
        return -1;
    }

    // 2 kinds of message: 1.async_t from worker, 2.callmodel
    uv_run(master_loop, UV_RUN_DEFAULT);

    reap_workers(workers, workers_num);
}
