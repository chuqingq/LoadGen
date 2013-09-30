#ifndef LS_SESSION_H_
#define LS_SESSION_H_

#include <map>
using namespace std;

#include "ls_task_script.h"
#include "ls_task_var.h"
#include "libuv/include/uv.h"

struct ls_session_s;
typedef int (*ls_session_process_t)(struct ls_session_s*);

typedef struct ls_session_s {
    int session_id;// TODO

    uv_loop_t* loop;

    const map<string, void*> *settings;// 引用master的 TODO ？？ 可有可无

    const ls_task_script_t* script;// TODO script只读
    int script_cur;

    map<string/* plugin_name */, void*> states;
    ls_task_var_t cur_vars;// const map<string, ls_var_t> vars;// session需要根据此初始化变量cur_vars、每次执行API前获取需要的变量更新cur_vars

    ls_session_process_t process;
    ls_session_process_t finish;    
} ls_session_t;


extern "C" int process_session(ls_session_t* session);

extern "C" int finish_session(ls_session_t* session);

#endif
