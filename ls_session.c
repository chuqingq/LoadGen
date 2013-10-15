#include <stdio.h>

#include "ls_master.h"
#include "ls_session.h"
#include "ls_plugin.h"

static int handle_session(ls_session_t* s) {
    const ls_task_script_entry_t* e = &(*s->script)[s->script_cur];
    ls_plugin_api_run_t api_run = (ls_plugin_api_run_t) e->api;
    map<string, string> vars;
    if ((api_run)(e->args, s, &vars) < 0)
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

int finish_session(ls_session_t* s) {
    printf("  ==== finish_session()\n");

    size_t i = 0;
    // 调用session中相关的所有plugin的session_destroy()
    for (map<string, void*>::iterator it = s->states.begin(); it != s->states.end(); ++it, ++i)
    {
        // ls_plugin_entry_t* e = &(master.plugins[it->first]);
        ls_plugin_entry_t* e = &master.plugins.entries[i];

        if ((e->session_destroy)(&(it->second)) < 0)
        {
            printf("ERROR failed to session_destroy()\n");
            return -1;
        }

        return 0;
    }

    delete s;
    return 0;
}
