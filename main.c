#include "lib/libuv/include/uv.h"

#include <unistd.h>

#include "ls_utils.h"
#include "ls_task_script.h"

#include "ls_master.h"
#include "ls_worker.h"
#include "ls_plugin.h"

#include "ls_task_callmodel.h"
#include "ls_task_setting.h"
#include "ls_task_var.h"

int main() {
    master.master_loop = uv_default_loop();

    log_init();

    master.num_workers = sysconf(_SC_NPROCESSORS_ONLN);
    if (master.num_workers < 1) {
        LOGE("ERROR workers num invalid\n");
    }

    if (load_task_callmodel(&master.callmodel) < 0) {
        LOGE("ERROR failed to load_task_callmodel.\n");
        return -1;
    }

    if (load_task_vars(&master.vars) < 0) {
        LOGE("ERROR failed to load_task_vars.\n");
        return -1;
    }

    // plugin_declare()/plugin_init()
    if (load_task_setting(&master) < 0) {
        LOGE("ERROR failed to load_task_setting.\n");
        return -1;
    }

    // api_init()
    if (load_task_script(&master.script) < 0) {
        LOGE("ERROR failed to load_task_script.\n");
        return -1;
    }

    // master_init()
    if (start_master(&master) < 0) {
        LOGE("ERROR failed to start_master.\n");
        return -1;
    }

    // worker_init()
    if (start_workers(&master) < 0) {
        LOGE("ERROR failed to start_workers.\n");
        return -1;
    }

    LOG("==== task is starting...\n");

    if (do_task_callmodel(&master.callmodel) < 0) {
        LOGE("ERROR failed to do_task_callmodel.\n");
        return -1;
    }

    uv_run(uv_default_loop(), UV_RUN_DEFAULT);

    // worker_terminate() 在worker_stop中调用
    if (reap_workers(&master) < 0) {
        LOGE("ERROR failed to reap_workers()\n");
    }

    // master_terminate()
    if (stop_master(&master) < 0) {
        LOGE("ERROR failed to stop_master()\n");
    }
    
    // api_terminate()
    if (unload_task_script(&master.script) < 0) {
        LOGE("ERROR failed to unload_task_script()\n");
    }

    // plugin_terminate
    if (unload_task_setting(&master) < 0) {
        LOGE("failed to unload_task_setting()\n");
    }
    
    if (unload_task_vars(&master.vars) < 0) {
        LOGE("ERROR failed to unload_task_vars.\n");
    }

    if (unload_task_callmodel(&master.callmodel) < 0) {
        LOGE("ERROR failed to load_task_callmodel.\n");
    }

    log_terminate();

    return 0;
}
