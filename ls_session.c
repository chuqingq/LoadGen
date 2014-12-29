#include <stdio.h>
#include <stdlib.h>

#include "ls_utils.h"
#include "ls_master.h"
#include "ls_session.h"
#include "ls_plugin.h"

// static int handle_session(ls_session_t* s) {
    

//     // TODO 处理vars
//     return 0;
// }

int process_session(ls_session_t* s) {
  start:
    s->script_cur = (s->script_cur + 1) % master.script.entries_num;

    const ls_task_script_entry_t* e = master.script.entries + s->script_cur;
    ls_plugin_api_t* api = (ls_plugin_api_t*) e->api;
    map<string, string> vars;
    int ret = (api->run)(e->args, s->plugin_states[api->plugin->plugin_index], &vars);
    switch (ret) {
        case 0: goto start;
        case -1: LOGE("  ERROR failed to run api [%s]\n", e->api->name); return -1;
        case 1: return 0;
    }
    return 0;
}

int finish_session(ls_session_t* s) {
    LOG("finish_session()\n");

    ls_plugin_t* plugin;
    // 调用session中相关的所有plugin的session_destroy()
    for (size_t i = 0; i < master.num_plugins; ++i) {
        plugin = master.plugins + i;

        if ((plugin->session_terminate)(s->plugin_states[i]) < 0) {
            LOG("ERROR failed to %s.session_destroy()\n", plugin->plugin_name);
            return -1;
        }
        s->plugin_states[i] = NULL;

        return 0;
    }

    free(s->plugin_states);
    s->plugin_states = NULL;

    delete s;
    s = NULL;

    return 0;
}
