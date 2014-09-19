#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

#include <errno.h>

#include "ls_utils.h"
#include "ls_config.h"
#include "ls_master.h"
#include "ls_task_callmodel.h"


int load_task_callmodel(ls_task_callmodel_t* callmodel) {
    LOG("load_task_callmodel()\n");

    const char* cm_file = "task/callmodel.json";

    char* buf;
    long len;
    FILE* f = fopen(cm_file, "r");
    if (NULL == f) {
        LOGE("Failed to fopen callmodel file[%s]: %s\n",
            cm_file, strerror(errno));
        return -1;
    }

    fseek(f, 0, SEEK_END);
    len = ftell(f);
    rewind(f);

    buf = (char*) malloc(len + 1);
    if (NULL == buf) {
        LOGE("Failed to malloc for call model: %s\n", strerror(errno));
        return -1;
    }

    len = fread(buf, 1, len, f);
    buf[len] = '\0';

    JSONNODE* n = json_parse(buf);

    for (JSONNODE_ITERATOR i = json_begin(n); i != json_end(n); ++i)
    {
        json_char* name = json_name(*i);
        if (strcmp(name, "type") == 0)
        {
            json_char* type = json_as_string(*i);
            if (strcmp(type, "CALLMODEL_VUSER") == 0)
            {
                callmodel->type = CALLMODEL_VUSER;
            }
            else if (strcmp(type, "CALLMODEL_CAPS"))
            {
                callmodel->type = CALLMODEL_CAPS;
            }
            else
            {
                LOGE("CALLMODEL type invalid\n");
            }
        }
        else if (strcmp(name, "init") == 0)
        {
            callmodel->init = json_as_int(*i);
        }
        else if (strcmp(name, "accelerate") == 0)
        {
            callmodel->accelerate = json_as_int(*i);
        }
        else if (strcmp(name, "dest") == 0)
        {
            callmodel->dest = json_as_int(*i);
        }
        else if (strcmp(name, "duration") == 0)
        {
            callmodel->duration = json_as_int(*i);
        }
        else
        {
            LOGE("call_model name invalid\n");
        }
        json_free(name);
    }
    callmodel->current = 0;

    return 0;
}

int unload_task_callmodel(ls_task_callmodel_t* callmodel) {
    LOG("load_task_callmodel()\n");
    // TODO
    return 0;
}

static void duration_timeout(uv_timer_t* handle, int status) {
    LOG("duration_timeout(): callmodel end\n");

    uv_timer_stop(handle);
    stop_workers(&master);

    uv_stop(handle->loop);/* 呼叫完了，停止 */
}

static void accelerate_per_sec(uv_timer_t* handle, int status) {
    LOG("  accelerate_per_sec()\n");

    ls_task_callmodel_t* cm = container_of(handle, struct ls_task_callmodel_s, accelerate_timer);

    // TODO 每秒钟需要增加的accelerate/CPU分配给worker
    // TODO callmodel_worker(config.worker_num);

    LOG("    accelerate=%d\n", cm->accelerate);
    LOG("    current=%d\n", cm->current);
    LOG("    dest=%d\n", cm->dest);

    // 启动cm->accelerate个session
    if (start_new_session(cm->accelerate) < 0)
    {
        LOG("ERROR failed to start_new_session()\n");
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
        LOG("  duration: %d\n", cm->duration);
        uv_timer_start(&(cm->duration_timer), duration_timeout, cm->duration*1000, 0);
    }
}


int do_task_callmodel(ls_task_callmodel_t* cm) {
    LOG("  do_task_callmodel()\n");

    // ls_master_t* master = container_of(cm, ls_master_t, callmodel);

    // 把呼叫模型中current从0改为init
    // TODO 启动init个session
    if (start_new_session(cm->init) < 0)
    {
        LOG("ERROR failed to start_new_session()\n");
        return -1;
    }

    cm->current = cm->init;

    uv_timer_t* t = &(cm->accelerate_timer);
    uv_timer_init(uv_default_loop(), t);
    uv_timer_start(t, accelerate_per_sec, 1000, 1000);

    return 0;
}
