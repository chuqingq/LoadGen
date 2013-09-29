#ifndef LS_WORKER_H_
#define LS_WORKER_H_

#include <map>
using namespace std;

#include "libuv/include/uv.h"

#include "ls_task_script.h"
#include "ls_task_setting.h"
#include "ls_task_var.h"


typedef struct {
    int session_id;// 暂时无用

    const map<string, void*> *settings;// 引用master的 TODO ？？ 可有可无

    ls_task_script_t cur_script;// session拷贝一份执行逻辑，需要标记走到哪一步了

    map<string/* plugin_name */, void*> states;
    ls_task_var_t cur_vars;// const map<string, ls_var_t> vars;// session需要根据此初始化变量cur_vars、每次执行API前获取需要的变量更新cur_vars
} ls_session_t;


typedef struct {
    uv_thread_t thread;
    uv_loop_t* worker_loop;

    uv_async_t master_async;
    uv_async_t worker_async;

    vector<ls_session_t> sessions;

} ls_worker_t;

int init_worker(ls_worker_t* w);
int notify_worker(const string& cmd);

int process_session(ls_session_t* session);
int worker_start_new_session(ls_worker_t* w, int num);

#endif
