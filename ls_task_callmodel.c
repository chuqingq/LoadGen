#include <stdio.h>
#include <stddef.h>

#include <cassert>
#include <fstream>
using namespace std;

#include "ls_utils.h"
#include "ls_config.h"
#include "ls_task_callmodel.h"


int load_task_callmodel(ls_task_callmodel_t* callmodel) {
    printf("====load_task_callmodel()\n");
    const char* cm_file = "task/callmodel.json";
    ifstream ifs;
    
    ifs.open(cm_file);
    assert(ifs.is_open());

    Json::Reader reader;
    Json::Value root;

    assert(reader.parse(ifs, root, false));

    // type
    if (root["type"].asString() == string("CALLMODEL_VUSER"))
    {
        callmodel->type = CALLMODEL_VUSER;
    }
    else
    {
        callmodel->type = CALLMODEL_CAPS;
    }

    // init // TODO 没有做有效性校验
    callmodel->init = root["init"].asInt();
    printf("\tinit=%d\n", callmodel->init);

    // accelerate
    callmodel->accelerate = root["accelerate"].asInt();
    printf("\taccelerate=%d\n", callmodel->accelerate);

    // dest
    callmodel->dest = root["dest"].asInt();
    printf("\tdest=%d\n", callmodel->dest);

    // duration
    callmodel->duration = root["duration"].asInt();
    printf("\tduration=%d\n", callmodel->duration);

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
    printf("==accelerate_per_sec()\n");

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
    printf("====do_task_callmodel()\n");

    uv_timer_t* t = &(cm->accelerate_timer);
    uv_timer_init(uv_default_loop(), t);
    uv_timer_start(t, accelerate_per_sec, 3000, 1000);

    return 0;
}
