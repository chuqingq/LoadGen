#include "ls_config.h"
#include "ls_script.h"
#include "ls_callmodel.h"
#include "ls_worker.h"

#include "uv.h"

uv_loop_t* master_loop;
ls_worker_t* workers;
ls_config_t config;

int spawn_workers() {
    int num_of_cpu = 2;/* TODO */
    workers = malloc(num_of_cpu * sizeof(*workers));

    for (int i = 0; i < num_of_cpu; ++i)
    {
        ls_worker_t* w = workers + i;

        if (uv_async_init(master_loop, w->master_async, master_async_todo) < 0) {
            return -1;/* TODO */
        }

        if (init_worker(w) < 0)
        {
            return -1;/* TODO */
        }
    }

    return 0;
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

    if (spawn_workers() < 0) {// 启动worker线程
        return -1;
    }

    if (do_callmodel(CallModel* callmodel) < 0) {
        return -1;
    }

    // 2 kinds of message: 1.async_t from worker, 2.callmodel
    uv_run(master_loop, UV_RUN_DEFAULT);
}
