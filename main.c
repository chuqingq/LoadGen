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
    ls_plugin_t plugins;
    map<string, JsonObj*> settings;


    master_loop = uv_default_loop();

    // -------------- 静态内容
    // 读取配置
    if (load_config(&config) < 0) {
        return -1;
    }

    // 加载协议
    if (load_plugins(&plugins) < 0) {
        return -1;
    }

    // -------------- 任务相关内容
    // 读取任务呼叫模型
    if (load_task_callmodel(&callmodel) < 0) {
        return -1;
    }

    // 读取任务设置并交给plugin处理
    if (load_task_settings(&settings) < 0) {
        return -1;
    }

    if (plugins_load_task_settings(&settings, &plugins) < 0) {// TODO
        return -1;
    }

    // 读取任务变量
    if (load_task_vars(&vars) < 0) {
        return -1;
    }

    // 读取任务脚本流程
    if (load_task_script(&script) < 0) {
        return -1;
    }

    // --------------- worker
    // 启动worker
    if (start_workers() < 0) {// 启动worker线程
        return -1;
    }

    // master按照呼叫模型分配呼叫
    if (do_callmodel(callmodel) < 0) {
        return -1;
    }

    // 2 kinds of message: 1.async_t from worker, 2.callmodel
    uv_run(master_loop, UV_RUN_DEFAULT);

    reap_workers();// TODO

    // 卸载协议
}
