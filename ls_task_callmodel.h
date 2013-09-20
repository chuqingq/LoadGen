#ifndef LS_TASK_CALLMODEL_H_
#define LS_TASK_CALLMODEL_H_


#include "libuv/include/uv.h"

#include "ls_worker.h"

typedef enum {
    CALLMODEL_VUSER = 0,
    CALLMODEL_CAPS
} ls_task_callmodel_type;

typedef struct ls_task_callmodel_s {
    ls_task_callmodel_type type;
    int init;       /* 初始数量 */
    int accelerate; /* 每秒变化数量 */
    int dest;       /* 目标值 */
    int duration;   /* 达到目标值后的呼叫时常 */

    int current;    /* 当前的呼叫数 */
    uv_timer_t accelerate_timer;
    uv_timer_t duration_timer;

    ls_worker_t* workers;
    int worker_num;
} ls_task_callmodel_t;

int load_task_callmodel(ls_task_callmodel_t* callmodel);

int do_task_callmodel(ls_task_callmodel_t* callmodel);

#endif
