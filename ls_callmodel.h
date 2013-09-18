
typedef enum {
    CALLMODEL_VUSER = 0;
    CALLMODEL_CAPS;
} ls_callmodel_type;

typedef struct ls_callmodel_s {
    ls_callmodel_type type;
    int init;       /* 初始数量 */
    int accelerate; /* 每秒变化数量 */
    int dest;       /* 目标值 */
    int duration;   /* 达到目标值后的呼叫时常 */

    int current;    /* 当前的呼叫数 */
    uv_timer_t accelerate_timer;
    uv_timer_t duration_timer;

    ls_worker_t* workers;
    int worker_num;
} ls_callmodel_t;

int read_callmodel(ls_callmodel_t* callmodel);

int do_callmodel(ls_callmodel_t* callmodel);
