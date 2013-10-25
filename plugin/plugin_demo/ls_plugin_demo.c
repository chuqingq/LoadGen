#include <stdio.h>
#include <stdlib.h>

#include <string>
#include <map>
#include <iostream>
using namespace std;

#include "ls_plugin.h"
#include "ls_worker.h"
#include "ls_session.h"
#include "ls_stats.h"

typedef struct {
    bool ignore_think_time;
} ls_plugin_demo_setting_t;

ls_plugin_demo_setting_t plugin_demo_setting;

typedef struct {
} ls_plugin_demo_state_t;

ls_plugin_demo_state_t plugin_demo_state;

static int master_init(struct ls_master_s*) {
    printf("  >>>> plugin_demo master_init()\n");
    return 0;
}

static int master_terminate(struct ls_master_s*) {
    printf("  >>>> plugin_demo master_terminate()\n");
    return 0;
}

static int plugin_script_init(ls_task_setting_t* /* setting*/) {
    printf("  >>>> plugin_script_init()\n");
    return 0;
}

static int plugin_script_terminate(ls_task_setting_t* /* setting */) {
    printf("  >>>> plugin_script_terminate()\n");
    return 0;
}

// static int plugin_task_init(const Json::Value* setting) {
static int plugin_task_init(/*ls_task_setting_t* setting, ls_task_script_t**/) {
    printf("  >>>> plugin_demo plugin_task_init()\n");

    // TODO
    // plugin_demo_setting.ignore_think_time = (*setting)["ignore_think_time"].asBool();
    return 0;
}

static int plugin_task_terminate(/*ls_task_setting_t*, ls_task_script_t**/) {
    printf("  >>>> plugin_demo plugin_task_terminate()\n");
    
    return 0;
}

static int worker_init(struct ls_worker_s*) {
    printf("  >>>> plugin_demo worker_init()\n");
    return 0;
}

static int worker_terminate(struct ls_worker_s*) {
    printf("  >>>> plugin_demo worker_terminate()\n");
    return 0;
}

// static int plugin_session_init(void** state) {
static int plugin_session_init(ls_session_t*) {
    printf("  >>>> plugin_demo plugin_session_init()\n");

    return 0;
}

// static int plugin_session_terminate(void** state) {
static int plugin_session_terminate(ls_session_t*) {
    printf("  >>>> plugin_demo plugin_session_terminate()\n");

    return 0;
}

// ls_think_time
static int ls_think_time_init(const Json::Value* json_args, void** args) {
    printf("  >>>> plugin_demo ls_think_time init()\n");
    int time = (*json_args)["time"].asInt();
    *args = (void*)time;
    printf("    time=%d\n", time);
    return 0;
}

static int ls_think_time_terminate(void** args) {
    printf("  >>>> plugin_demo ls_think_time terminate()\n");
    return 0;
}

static void timer_cb(uv_timer_t* handle, int status) {
    printf("  >>>> plugin_demo ls_think_time timer_cb()\n");
    // 根据handle获取到session
    ls_session_t* s = (ls_session_t*) handle->data;
    uv_timer_stop(handle);
    delete handle;

    (s->process)(s);
}

static int ls_think_time(const void* args, ls_session_t* session, map<string, string> * vars) {
    int time = (int)args;
    printf("  >>>> plugin_demo ls_think_time(%d)\n", time);

    printf("  ignore_think_time=%d\n", (int)plugin_demo_setting.ignore_think_time);

    uv_timer_t* timer = new uv_timer_t;// delete at line 77
    ls_worker_t* w = (ls_worker_t*)session->worker;
    uv_timer_init(w->worker_loop, timer);

    timer->data = session;
    uv_timer_start(timer, timer_cb, time, 0);

    return 0;
}

// ls_error_message
static int ls_error_message_init(const Json::Value* json_args, void** args) {
    printf("  >>>> plugin_demo ls_error_message init()\n");
    *args = (void*)json_args;
    return 0;
}

static int ls_error_message_terminate(void** args) {
    printf("  >>>> plugin_demo ls_error_message terminate()\n");

    return 0;
}

static int ls_error_message(const void* args, ls_session_t* session, map<string, string> * vars) {
    printf("  >>>> plugin_demo ls_error_message(%d)\n", 1);
    Json::Value* json_args = (Json::Value*)args;
    printf("    ls_error_message(): %s\n", (*json_args)["message"].asString().c_str());
    return 0;
}


// ls_start_transaction
static int ls_start_transaction_init(const Json::Value* json_args, void** args) {
    printf("  >>>> plugin_demo ls_start_transaction init()\n");
    *args = (void*)json_args;
    return 0;
}

static int ls_start_transaction_terminate(void** args) {
    printf("  >>>> plugin_demo ls_start_transaction terminate()\n");

    return 0;
}

static int ls_start_transaction(const void* args, ls_session_t* session, map<string, string> * vars) {
    printf("  >>>> plugin_demo ls_start_transaction(%d)\n", 1);
    // Json::Value* json_args = (Json::Value*)args;
    // printf("    ls_start_transaction(): %s\n", (*json_args)["message"].asString().c_str());
    return 0;
}

// ls_end_transaction
static int ls_end_transaction_init(const Json::Value* json_args, void** args) {
    printf("  >>>> plugin_demo ls_end_transaction init()\n");
    // *args = (void*)json_args;
    return 0;
}

static int ls_end_transaction_terminate(void** args) {
    printf("  >>>> plugin_demo ls_end_transaction terminate()\n");

    return 0;
}

static int ls_end_transaction(const void* args, ls_session_t* session, map<string, string> * vars) {
    printf("  >>>> plugin_demo ls_end_transaction(%d)\n", 1);
    // Json::Value* json_args = (Json::Value*)args;
    // printf("    ls_end_transaction(): %s\n", (*json_args)["message"].asString().c_str());
    return 0;
}

static int plugin_demo_stats_handle(void* one_data, void* state) {
    printf("  >>>> plugin_demo_stats_handle()\n");
    // TODO
    return 0;
}

static int plugin_demo_stats_output(void* state) {
    printf("  >>>> plugin_demo_stats_output()\n");
    // TODO
    return 0;
}

extern "C" int plugin_declare(/* const char** plugin_name, */ls_plugin_t* plugin_entry) {
    char* plugin_name = (char*)"ls_plugin_demo";
    printf("  >>>> plugin_declare(%s)\n", plugin_name);

    // 1.plugin_name
    plugin_entry->plugin_name = plugin_name;

    // 2.callbacks
    plugin_entry->master_init = master_init;
    plugin_entry->master_terminate = master_terminate;

    plugin_entry->script_init = plugin_script_init;
    plugin_entry->script_terminate = plugin_script_terminate;

    plugin_entry->task_init = plugin_task_init;
    plugin_entry->task_terminate = plugin_task_terminate;

    plugin_entry->worker_init = worker_init;
    plugin_entry->worker_terminate = worker_terminate;

    plugin_entry->session_init = plugin_session_init;
    plugin_entry->session_terminate = plugin_session_terminate;

    // 3.stats
    plugin_entry->stats_num = 1;
    plugin_entry->stats = (ls_stats_entry_t*)malloc(plugin_entry->stats_num * sizeof(ls_stats_entry_t));
    if (plugin_entry->stats == NULL) {
        printf("  ERRROR plugin_demo failed to malloc stats\n");
        return -1;
    }

    plugin_entry->stats[0].state = NULL;
    plugin_entry->stats[0].handler = plugin_demo_stats_handle;
    plugin_entry->stats[0].output = plugin_demo_stats_output;

    // 4.apis
    plugin_entry->num_apis = 4;
    plugin_entry->apis = (ls_plugin_api_t*)malloc(plugin_entry->num_apis * sizeof(ls_plugin_api_t));

    // ls_think_time
    plugin_entry->apis[0].name = (char*)"ls_think_time";
    plugin_entry->apis[0].init = ls_think_time_init;
    plugin_entry->apis[0].run = ls_think_time;
    plugin_entry->apis[0].terminate = ls_think_time_terminate;

    // ls_error_message
    plugin_entry->apis[1].name = (char*)"ls_error_message";
    plugin_entry->apis[1].init = ls_error_message_init;
    plugin_entry->apis[1].run = ls_error_message;
    plugin_entry->apis[1].terminate = ls_error_message_terminate;

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

    printf("  >>>> end plugin_declare()\n");
    return 0;
}
