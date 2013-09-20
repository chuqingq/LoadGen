#include <stddef.h>

#include "ls_utils.h"
#include "ls_config.h"
#include "ls_task_callmodel.h"


int load_task_callmodel(ls_task_callmodel_t* callmodel) {
    /* TODO 从callmodel.json中读取 */
    callmodel->type = CALLMODEL_VUSER;

    callmodel->init = 20;
    callmodel->accelerate = 5;
    callmodel->dest = 100;

    callmodel->duration = 120;

    return 0;
}

static void duration_timeout(uv_timer_t* handle, int status) {
    // TODO 通知worker停止
    // ls_task_callmodel_t* cm = container_of(handle, ls_task_callmodel_t, duration_timer);
    // TODO notify_worker("stop");

    uv_timer_stop(handle);

    // TODO 呼叫完了，是否要停止？
}

static void accelerate_per_sec(uv_timer_t* handle, int status) {
    ls_task_callmodel_t* cm = container_of(handle, struct ls_task_callmodel_s, accelerate_timer);

    // TODO 每秒钟需要增加的accelerate/CPU分配给worker
    // TODO callmodel_worker(config.worker_num);

    cm->current += cm->accelerate;

    // 如果当前值达到dest，则关闭此timer，开启另一个duration_timer
    if (cm->current == cm->dest
        || cm->accelerate > 0 && cm->current > cm->dest
        || cm->accelerate < 0 && cm->current < cm->dest)
    {
        uv_timer_stop(&(cm->accelerate_timer));

        uv_timer_init(uv_default_loop(), &(cm->duration_timer));
        uv_timer_start(&(cm->duration_timer), duration_timeout, cm->duration, 0);
    }
}


int do_task_callmodel(ls_task_callmodel_t* cm) {
    uv_timer_t* t = &(cm->accelerate_timer);
    uv_timer_init(uv_default_loop(), t);
    uv_timer_start(t, accelerate_per_sec, 3000, 1000);

    return 0;
}
