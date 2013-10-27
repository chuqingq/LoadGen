#include <stdio.h>
#include <stddef.h>

#include <cassert>
#include <fstream>
using namespace std;

#include "ls_utils.h"
#include "ls_config.h"
#include "ls_master.h"
#include "ls_task_callmodel.h"


int load_task_callmodel(ls_task_callmodel_t* callmodel) {
    LOG("==== load_task_callmodel()\n");
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
    LOG("  init=%d\n", callmodel->init);

    // accelerate
    callmodel->accelerate = root["accelerate"].asInt();
    LOG("  accelerate=%d\n", callmodel->accelerate);

    // dest
    callmodel->dest = root["dest"].asInt();
    LOG("  dest=%d\n", callmodel->dest);

    // duration
    callmodel->duration = root["duration"].asInt() * 1000;
    LOG("  duration=%d\n", callmodel->duration);

    // current 当前设置为0，第一次直接控制时设置为init值
    callmodel->current = 0;

    return 0;
}

static void duration_timeout(uv_timer_t* handle, int status) {
    LOG("==== duration_timeout(): callmodel end\n");

    uv_timer_stop(handle);
    stop_workers(&master);

    uv_stop(handle->loop);/* 呼叫完了，停止 */
}

static void accelerate_per_sec(uv_timer_t* handle, int status) {
    LOG("  ==== accelerate_per_sec()\n");

    ls_task_callmodel_t* cm = container_of(handle, struct ls_task_callmodel_s, accelerate_timer);

    // TODO 每秒钟需要增加的accelerate/CPU分配给worker
    // TODO callmodel_worker(config.worker_num);

    LOG("  accelerate=%d\n", cm->accelerate);
    LOG("  current=%d\n", cm->current);
    LOG("  dest=%d\n", cm->dest);

    // 启动cm->accelerate个session
    if (start_new_session(cm->accelerate) < 0)
    {
        LOG("  Failed to start_new_session()\n");
        return;// TODO 退出
    }
    cm->current += cm->accelerate;

    // 如果当前值达到dest，则关闭此timer，开启另一个duration_timer
    if ((cm->current == cm->dest)
        || (cm->accelerate > 0 && cm->current > cm->dest)
        || (cm->accelerate < 0 && cm->current < cm->dest))
    {
        uv_timer_stop(&(cm->accelerate_timer));

        uv_timer_init(uv_default_loop(), &(cm->duration_timer));
        uv_timer_start(&(cm->duration_timer), duration_timeout, cm->duration, 0);
    }
}


int do_task_callmodel(ls_task_callmodel_t* cm) {
    LOG("==== do_task_callmodel()\n");

    // ls_master_t* master = container_of(cm, ls_master_t, callmodel);

    // 把呼叫模型中current从0改为init
    // TODO 启动init个session
    if (start_new_session(cm->init) < 0)
    {
        LOG("  Failed to start_new_session()\n");
        return -1;
    }

    cm->current = cm->init;

    uv_timer_t* t = &(cm->accelerate_timer);
    uv_timer_init(uv_default_loop(), t);
    uv_timer_start(t, accelerate_per_sec, 1000, 1000);

    return 0;
}
