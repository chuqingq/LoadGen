#include <stdio.h>

#include "ls_session.h"
#include "ls_plugin.h"

static int handle_session(ls_session_t* s) {
    const ls_task_script_entry_t* e = &(*s->script)[s->script_cur];
    ls_plugin_api_t api = (ls_plugin_api_t) e->api;
    map<string, string> vars;
    if ((api)(e->args, s, &vars) < 0)
    {
        printf("==== API [%s] error\n", e->api_name.c_str());
        return -1;
    }

    // TODO 处理vars
    return 0;
}

int process_session(ls_session_t* s) {
    // printf("==== process_session()\n");
    if (s->script_cur == s->script->size()-1)
    {
        s->script_cur = -1;
    }

    s->script_cur++;

    return handle_session(s);
}

int finish_session(ls_session_t* session) {
    printf("==== finish_session()\n");

    // TODO 停止一个session，并从worker的sessions中删除。flag表示结果成功还是失败
    delete session;
    return -1;
}
