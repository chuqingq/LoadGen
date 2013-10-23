#include <stdio.h>
#include <stdlib.h>

#include "ls_master.h"
#include "ls_session.h"
#include "ls_plugin.h"

static int handle_session(ls_session_t* s) {
    const ls_task_script_entry_t* e = s->script->entries + s->script_cur;
    ls_plugin_api_t* api = (ls_plugin_api_t*) e->api;
    map<string, string> vars;
    if ((api->run)(e->args, s, &vars) < 0)
    {
        printf("==== API [%s] error\n", e->api_name.c_str());
        return -1;
    }

    // TODO 处理vars
    return 0;
}

int process_session(ls_session_t* s) {
    if (s->script_cur == s->script->entries_num-1)
    {
        s->script_cur = -1;
    }

    s->script_cur++;

    return handle_session(s);
}

int finish_session(ls_session_t* s) {
    printf("  ==== finish_session()\n");

    ls_plugin_t* e;
    // 调用session中相关的所有plugin的session_destroy()
    for (size_t i = 0; i < master.config.plugins_num; ++i)
    {
        e = master.plugins + i;

        // if ((e->session_terminate)(s->states + i) < 0)
        if ((e->session_terminate)(s) < 0)
        {
            printf("ERROR failed to session_destroy()\n");
            return -1;
        }

        return 0;
    }

    free(s->states);
    s->states = NULL;

    delete s;
    s = NULL;

    return 0;
}
