#ifndef LS_WORKER_H_
#define LS_WORKER_H_

#include <map>
using namespace std;

#include "libuv/include/uv.h"

#include "ls_task_script.h"
#include "ls_task_setting.h"
#include "ls_task_var.h"

// typedef ls_session_s {
//     // 只读
//     ls_script_t* script;
//     ls_task_setting_t* settings;
//     ls_data_t* data;
// 
//     ls_worker_t* worker;// 所属的worker
//     // 脚本当前执行到哪里了
//     ls_session_script_t* cur_script;
//     ls_session_rtsetting_t* cur_settigs;
//     ls_session_data_t* cur_data;
// } ls_session_t;

typedef struct {
    int session_id;// 暂时无用

    const map<string, void*> *settings;// 引用master的 TODO ？？ 可有可无

    ls_task_script_t cur_script;// session拷贝一份执行逻辑，需要标记走到哪一步了

    map<string, void*> states;
    ls_task_var_t cur_vars;// const map<string, ls_var_t> vars;// session需要根据此初始化变量cur_vars、每次执行API前获取需要的变量更新cur_vars
} ls_session_t;


// typedef ls_worker_s {
//     uv_thread_t thread;
//     uv_loop_t* worker_loop;
// 
//     uv_async_t worker_async;/* master向worker通信 */
//     uv_async_t master_async;/* worker向worker通信 */
// 
//     // 多个session
//     vector<ls_session_t*> sessions;
// } ls_worker_t;

typedef struct {
    uv_thread_t thread;
    uv_loop_t* worker_loop;

    uv_async_t master_async;
    uv_async_t worker_async;

    vector<ls_session_t> sessions;

} ls_worker_t;


int process_session(ls_session_t* session);

int notify_worker(const string& cmd);

int worker_start_new_session(ls_worker_t* w, int num);

#endif
