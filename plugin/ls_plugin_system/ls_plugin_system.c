#include <stdio.h>
#include <stdlib.h>

#include <string>
#include <map>
#include <iostream>
using namespace std;

#include "ls_utils.h"
#include "ls_plugin.h"
#include "ls_worker.h"
#include "ls_session.h"


typedef struct {
    uint64_t count;
    uint64_t total;
} tran_t;
typedef map<string, tran_t> system_trans_t;

typedef struct {
    bool ignore_think_time;
} system_setting_t;

system_setting_t system_setting;

typedef struct {
    system_trans_t trans;// 事物统计
} system_worker_state_t;

typedef struct {
    map<string, uint64_t> trans;
} system_session_state_t;// TODO session

// ls_system_state_t system_state;

const static char* plugin_name = "ls_plugin_system";
static size_t plugin_id;


// static int system_trans_add(system_trans_t* worker_trans, const char* trans_name, int trans_flag, uint64_t trans_duration) {
//     // TODO
//     system_trans_t::iterator it = worker_trans->find(trans_name);
//     if (it != worker_trans->end())
//     {
//         printf("ERROR system_trans_add\n");
//         return -1;
//     }

//     it->second.count++;
//     it->second.total += trans_duration;

//     printf("  avg = %llu\n", it->second.total/it->second.count);

//     return 0;
// }

// ls_start_transaction
// static int ls_start_transaction_init(const Json::Value* json_args, void** args) {
static int ls_start_transaction_init(const JSONNODE* json_args, void** args) {
    LOGP("%s.ls_start_transaction_init()\n", plugin_name);

    *args = (void*)json_args;
    return 0;
}

static int ls_start_transaction_terminate(void** args) {
    LOGP("%s.ls_start_transaction_terminate()\n", plugin_name);

    return 0;
}

static int ls_start_transaction(const void* args, ls_session_t* session, map<string, string> * vars) {
    LOGP("%s.ls_start_transaction()\n", plugin_name);

    // JSONNODE** json_args = (JSONNODE**)args;
    // string name = (*json_args)["transaction_name"].asString();
    string name = string("transaction_name_todo");

    uint64_t start = uv_now(session->worker->worker_loop);
    LOGP("  start: %llu\n", start);

    system_session_state_t* state = (system_session_state_t*)session->plugin_states[plugin_id];
    state->trans.insert(make_pair(name, start));

    process_session(session);
    return 0;
}

// ls_end_transaction
static int ls_end_transaction_init(const JSONNODE* json_args, void** args) {
    LOGP("%s.ls_end_transaction_init()\n", plugin_name);

    *args = (void*)json_args;
    return 0;
}

static int ls_end_transaction_terminate(void** args) {
    LOGP("%s.ls_end_transaction_terminate()\n", plugin_name);

    return 0;
}

static int ls_end_transaction(const void* args, ls_session_t* session, map<string, string> * vars) {
    LOGP("%s.ls_end_transaction()\n", plugin_name);

    // JSONNODE** json_args = (JSONNODE**)args;
    // string name = (*json_args)["transaction_name"].asString();
    string name = "transaction_name_todo";

    uint64_t stop = uv_now(session->worker->worker_loop);

    system_session_state_t* state = (system_session_state_t*)session->plugin_states[plugin_id];
    uint64_t start = state->trans[name];
    LOGP("  start:%llu, stop:%llu\n", start, stop);
    state->trans.erase(name);

    LOGP("  ls_end_transaction(): %s: %lld\n", name.c_str(), stop-start);
    // TODO 向worker统计
    // ls_worker_t* w = session->worker;
    // system_worker_state_t* workerstate = w->plugin_states[plugin_id];


    process_session(session);
    return 0;
}


static int master_init(struct ls_master_s*, const JSONNODE* setting) {
    LOGP("%s.master_init()\n", plugin_name);
    return 0;
}

static int master_terminate(struct ls_master_s*) {
    LOGP("%s.master_terminate()\n", plugin_name);
    return 0;
}

// static int plugin_script_init(void* /* setting*/) {
//     LOGP("%s.script_init()\n", plugin_name);
//     return 0;
// }

// static int plugin_script_terminate(void* /* setting */) {
//     LOGP("%s.script_terminate()\n", plugin_name);
//     return 0;
// }

// static int plugin_task_init(const Json::Value* setting) {
static int plugin_task_init(/*ls_task_setting_t* setting, ls_task_script_t**/) {
    LOGP("%s.task_init()\n", plugin_name);

    // TODO
    // system_setting.ignore_think_time = (*setting)["ignore_think_time"].asBool();
    return 0;
}

static int plugin_task_terminate(/*ls_task_setting_t*, ls_task_script_t**/) {
    LOGP("%s.task_terminate()\n", plugin_name);
    
    return 0;
}

static int worker_init(struct ls_worker_s* w) {
    LOGP("%s.worker_init()\n", plugin_name);

    // system_worker_state_t* state = (system_worker_state_t*)malloc(sizeof(system_worker_state_t));
    // if (state == NULL)
    // {
    //     LOGP("ERROR failed to malloc system_worker_state\n");
    //     return -1;
    // }

    // state->trans = system_trans_t();

    // w->plugin_states[plugin_id] = (void*)state;
    return 0;
}

static int worker_terminate(struct ls_worker_s* w) {
    LOGP("%s.worker_terminate()\n", plugin_name);

    // free(w->plugin_states[plugin_id]);
    // w->plugin_states[plugin_id] = NULL;
    return 0;
}

static int plugin_session_init(ls_session_t* session) {
    LOGP("%s.session_init()\n", plugin_name);

    system_session_state_t* state = (system_session_state_t*)malloc(sizeof(system_session_state_t));
    if (state == NULL)
    {
        LOGP("ERROR failed to malloc system_session_state\n");
        return -1;
    }
    state->trans = map<string, uint64_t>();

    session->plugin_states[plugin_id] = (void*)state;
    return 0;
}

static int plugin_session_terminate(ls_session_t* session) {
    LOGP("%s.session_terminate()\n", plugin_name);

    free(session->plugin_states[plugin_id]);
    session->plugin_states[plugin_id] = NULL;

    return 0;
}

// ls_think_time
// static int ls_think_time_init(const S::Value* json_args, void** args) {
static int ls_think_time_init(const JSONNODE* json_args, void** args) {
    LOGP("%s.ls_think_time_init()\n", plugin_name);

    // int time = (*json_args)["time"].asInt();
    int time = 1000;
    *args = (void*)time;
    LOGP("  time=%d\n", time);
    return 0;
}

static int ls_think_time_terminate(void** args) {
    LOGP("%s.ls_think_time_terminate()\n", plugin_name);

    return 0;
}

static void timer_cb(uv_timer_t* handle, int status) {
    LOGP("  %s.timer_cb()\n", plugin_name);

    // 根据handle获取到session
    ls_session_t* s = (ls_session_t*) handle->data;
    uv_timer_stop(handle);
    delete handle;

    process_session(s);
}

static int ls_think_time(const void* args, ls_session_t* session, map<string, string> * vars) {
    int time = (int)args;
    LOGP("%s.ls_think_time(%d)\n", plugin_name, time);
    LOGP("  ignore_think_time=%d\n", (int)system_setting.ignore_think_time);

    uv_timer_t* timer = new uv_timer_t;// delete at line 77
    ls_worker_t* w = (ls_worker_t*)session->worker;
    uv_timer_init(w->worker_loop, timer);

    timer->data = session;
    uv_timer_start(timer, timer_cb, time, 0);

    return 0;
}

// ls_output_message
static int ls_output_message_init(const JSONNODE* json_args, void** args) {
    LOGP("%s.ls_output_message_init()\n", plugin_name);

    *args = (void*)json_args;
    return 0;
}

static int ls_output_message_terminate(void** args) {
    LOGP("%s.ls_output_message_terminate()\n", plugin_name);

    return 0;
}

static int ls_output_message(const void* args, ls_session_t* session, map<string, string> * vars) {
    LOGP("%s.ls_output_message()\n", plugin_name);

    // JSONNODE** json_args = (JSONNODE**)args;
    // LOGP("    ls_output_message output: %s\n", (*json_args)["message"].asString().c_str());
    LOGP("    ls_output_message output todo\n");

    process_session(session);
    return 0;
}


extern "C" int plugin_declare(ls_plugin_t* plugin_entry) {
    LOGP("%s.plugin_declare()\n", plugin_name);

    // 1.plugin_name
    plugin_id = plugin_entry->plugin_index;
    plugin_entry->plugin_name = plugin_name;

    // 2.callbacks
    plugin_entry->master_init = master_init;
    plugin_entry->master_terminate = master_terminate;

    // plugin_entry->script_init = plugin_script_init;
    // plugin_entry->script_terminate = plugin_script_terminate;

    plugin_entry->task_init = plugin_task_init;
    plugin_entry->task_terminate = plugin_task_terminate;

    plugin_entry->worker_init = worker_init;
    plugin_entry->worker_terminate = worker_terminate;

    plugin_entry->session_init = plugin_session_init;
    plugin_entry->session_terminate = plugin_session_terminate;

    // 4.apis
    plugin_entry->num_apis = 4;
    plugin_entry->apis = (ls_plugin_api_t*)malloc(plugin_entry->num_apis * sizeof(ls_plugin_api_t));

    // ls_think_time
    plugin_entry->apis[0].name = (char*)"ls_think_time";
    plugin_entry->apis[0].init = ls_think_time_init;
    plugin_entry->apis[0].run = ls_think_time;
    plugin_entry->apis[0].terminate = ls_think_time_terminate;

    // ls_output_message
    plugin_entry->apis[1].name = (char*)"ls_output_message";
    plugin_entry->apis[1].init = ls_output_message_init;
    plugin_entry->apis[1].run = ls_output_message;
    plugin_entry->apis[1].terminate = ls_output_message_terminate;

    // ls_start_transaction
    plugin_entry->apis[2].name = (char*)"ls_start_transaction";
    plugin_entry->apis[2].init = ls_start_transaction_init;
    plugin_entry->apis[2].run = ls_start_transaction;
    plugin_entry->apis[2].terminate = ls_start_transaction_terminate;

    // ls_end_transaction
    plugin_entry->apis[3].name = (char*)"ls_end_transaction";
    plugin_entry->apis[3].init = ls_end_transaction_init;
    plugin_entry->apis[3].run = ls_end_transaction;
    plugin_entry->apis[3].terminate = ls_end_transaction_terminate;

    return 0;
}
