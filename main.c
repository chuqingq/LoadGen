#include <stdio.h>

#include "libuv/include/uv.h"

#include "ls_task_script.h"

#include "ls_master.h"
#include "ls_config.h"
#include "ls_worker.h"
#include "ls_plugin.h"

#include "ls_task_callmodel.h"
#include "ls_task_setting.h"
#include "ls_task_var.h"

int main() {
    master.master_loop = uv_default_loop();

    // -------------- 静态内容
    // 读取配置
    if (load_config(&(master.config)) < 0) {
        printf("Failed to load_config.\n");
        return -1;
    }

    // 加载协议 // plugin_load
    if (load_plugins(&(master.plugins)) < 0) {
        printf("Failed to load_plugins.\n");
        return -1;
    }

    // -------------- 任务相关内容
    // 读取任务呼叫模型
    if (load_task_callmodel(&(master.callmodel)) < 0) {
        printf("Failed to load_task_callmodel.\n");
        return -1;
    }

    // 1. 读取任务设置并交给plugin处理
    if (load_task_setting(&(master.settings)) < 0) {
        printf("Failed to load_task_setting.\n");
        return -1;
    }

    // 2. 读取任务变量
    if (load_task_vars(&(master.vars)) < 0) {
        printf("Failed to load_task_vars.\n");
        return -1;
    }

    // 3. 读取任务脚本流程
    if (load_task_script(&(master.script)) < 0) {
        printf("Failed to load_task_script.\n");
        return -1;
    }

    // -------------- 插件加载
    // 插件加载任务设置 // task_init()
    if (plugins_load_task_setting(&(master.settings), &(master.plugins)) < 0) {
        printf("Failed to plugins_load_task_setting.\n");
        return -1;
    }

    // 插件加载任务脚本
    if (plugins_load_task_script(&(master.script), &(master.plugins)) < 0) {
        printf("Failed to plugins_load_task_script.\n");
        return -1;
    }

    // ---------------  启动worker
    // 启动worker
    if (start_workers(&master) < 0) {// 启动worker线程，注册master和worker的交互方式
        printf("Failed to start_workers.\n");
        return -1;
    }

    printf("==== task will start...\n");

    // ----------------  启动呼叫
    // master按照呼叫模型分配呼叫
    if (do_task_callmodel(&(master.callmodel)) < 0) {
        printf("Failed to do_task_callmodel.\n");
        return -1;
    }

    // 2 kinds of message: 1.async_t from worker, 2.callmodel
    uv_run(uv_default_loop(), UV_RUN_DEFAULT);

    if (reap_workers(&master) < 0) {
        printf("ERROR failed to reap_workers()\n");
        return -1;
    }

    if (plugins_unload_task_script(&(master.script), &(master.plugins)) < 0)
    {
        printf("ERROR failed to plugins_unload_task_script()\n");
        return -1;
    }

    if (plugins_unload_task_setting(&(master.plugins)) < 0) // task_terminate()
    {
        printf("ERROR failed to plugins_unload_task_setting()\n");
        return -1;
    }

    if (unload_task_script(&master.script) < 0)
    {
        printf("ERROR failed to unload_task_script()\n");
        return -1;
    }

    if (unload_plugins(&(master.plugins)) < 0) // plugin_unload
    {
        printf("ERROR failed to unload_plugins()\n");
        return -1;
    }

    if (unload_config(&master.config) < 0)
    {
        printf("ERROR failed to unload_config()\n");
        return -1;
    }

    return 0;
}
