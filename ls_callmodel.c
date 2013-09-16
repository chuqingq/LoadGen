
#include "ls_callmodel.h"

int read_callmodel(ls_callmodel_t* callmodel) {
    /* TODO 从callmodel.json中读取 */
    callmodel->type = CALLMODEL_VUSER;

    callmodel->init = 20;
    callmodel->accelerate = 5;
    callmodel->dest = 100;

    callmodel->duration = 120;

    return 0;
}

static void duration_timeout(uv_timer_t* handle, int status) {
    // 通知worker停止
    uv_timer_stop(handle);
}

static void accelerate_per_sec(uv_timer_t* handle, int status) {
    ls_callmodel_t* cm = container_of(handle, struct ls_call_model_s, accelerate_timer);

    // TODO 每秒钟需要增加的accelerate/CPU分配给worker
    callmodel_worker(N);

    cm->current += cm->accelerate;

    // 如果当前值达到dest，则关闭此timer，开启另一个duration_timer
    if (cm->current == cm->dest
        || cm->accelerate > 0 && cm->current > cm->dest
        || cm->accelerate < 0 && cm->current < cm->dest)
    {
        uv_timer_stop(cm->accelerate_timer);

        uv_timer_init(uv_default_loop(), &(cm->duration_timer));
        uv_timer_start(&(cm->duration_timer), cm->duration, 0);
    }
}


int do_callmodel(ls_callmodel_t* cm) {
    uv_timer_t* t = &(cm->accelerate_timer);
    uv_timer_init(uv_default_loop(), t);
    uv_timer_start(t, accelerate_per_sec, 3000, 1000);

    return 0;
}
