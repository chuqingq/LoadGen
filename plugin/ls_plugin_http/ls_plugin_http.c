#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <string>
#include <map>
#include <iostream>
using namespace std;

#include "ls_utils.h"
#include "ls_plugin.h"
#include "ls_master.h"
#include "ls_worker.h"
#include "ls_session.h"


typedef struct {
    // bool ignore_think_time;
} http_setting_t;

http_setting_t http_setting;

typedef struct {
    // 
} http_worker_state_t;

typedef struct {
	ls_session_t* session;
} http_session_state_t;


const static char* plugin_name = "ls_plugin_http";
static size_t plugin_id;


static int ls_http_request_init(const JSONNODE* json_args, void** args) {
    LOGP("%s.ls_http_request_init()\n", plugin_name);

    // string* tran_name = NULL;
    // for (JSONNODE_ITERATOR i = json_begin((JSONNODE*)json_args); i != json_end((JSONNODE*)json_args); ++i) {
    //     json_char* name = json_name(*i);
    //     if (strcmp(name, "transaction_name") == 0) {
    //         tran_name = new string(json_as_string(*i));
    //     }
    //     json_free(name);
    // }
    // if (NULL == tran_name) {
    //     LOGP("ERROR failed to get param 'transaction_name'\n");
    //     return -1;
    // }
    // *args = tran_name;
    
    return 0;
}


static int ls_http_request_terminate(void** args) {
    LOGP("%s.ls_http_request_terminate()\n", plugin_name);
    // delete((string*)(*args));
    // *args = NULL;
    return 0;
}


static int ls_http_request(const void* args, void* sessionstate, map<string, string> * vars) {
    LOGP("%s.ls_http_request()\n", plugin_name);
	http_session_state_t* state = (http_session_state_t*)sessionstate;
    //    string* tran_name = (string*)args;

    //    uint64_t start = uv_now(state->session->worker->worker_loop);
    //    LOGP("  tran_name: %s; start: %llu\n", tran_name->c_str(), start);

    //    state->trans.insert(make_pair(*tran_name, start));
    process_session(state->session);
    return 0;
}


static int plugin_init(const JSONNODE* setting) {
    LOGP("%s.plugin_init()\n", plugin_name);
    return 0;
}

static int plugin_terminate() {
    LOGP("%s.plugin_terminate()\n", plugin_name);
    return 0;
}


static int master_init(struct ls_master_s* m) {
    LOGP("%s.master_init()\n", plugin_name);
    // // TODO 1. 读取setting到本地的结构

    // // 2. 读注册统计回调
    // if (uv_timer_init(m->master_loop, &trans_stats.stats_timer) < 0) {
    //     LOGE("trans_stats_timer init error\n");
    //     return -1;
    // }
    // trans_stats.master = m;

    // if (uv_timer_start(&trans_stats.stats_timer, collect_trans_stats, 100, 1000) < 0) {
    //     LOGE("trans_stats_timer start error\n");
    //     return -1;
    // }
    
    return 0;
}


static int master_terminate(struct ls_master_s*) {
    LOGP("%s.master_terminate()\n", plugin_name);
    return 0;
}


static int worker_init(struct ls_worker_s* w) {
    LOGP("%s.worker_init()\n", plugin_name);
    return 0;
}

static int worker_terminate(struct ls_worker_s* w) {
    LOGP("%s.worker_terminate()\n", plugin_name);
    return 0;
}


static int session_init(ls_session_t* session, void** sessionstate) {
    LOGP("%s.session_init()\n", plugin_name);

    http_session_state_t* state = (http_session_state_t*)malloc(sizeof(http_session_state_t));
    if (state == NULL)
    {
        LOGP("ERROR failed to malloc http_session_state\n");
        return -1;
    }
    state->session = session;

    *sessionstate = (void*)state;
    return 0;
}


static int session_terminate(void* sessionstate) {
    LOGP("%s.session_terminate()\n", plugin_name);
    free(sessionstate);
    return 0;
}


extern "C" int plugin_declare(ls_plugin_t* plugin_entry) {
    LOGP("%s.plugin_declare()\n", plugin_name);

    // 1.plugin_name
    plugin_id = plugin_entry->plugin_index;
    plugin_entry->plugin_name = plugin_name;

    // 2.callbacks
    plugin_entry->plugin_init = plugin_init;
    plugin_entry->plugin_terminate = plugin_terminate;
    
    plugin_entry->master_init = master_init;
    plugin_entry->master_terminate = master_terminate;

    plugin_entry->worker_init = worker_init;
    plugin_entry->worker_terminate = worker_terminate;

    plugin_entry->session_init = session_init;
    plugin_entry->session_terminate = session_terminate;

    // 4.apis
    plugin_entry->num_apis = 1;
    plugin_entry->apis = (ls_plugin_api_t*)malloc(plugin_entry->num_apis * sizeof(ls_plugin_api_t));

    // ls_http_request
    plugin_entry->apis[0].plugin = plugin_entry;
    plugin_entry->apis[0].name = (char*)"ls_http_request";
    plugin_entry->apis[0].init = ls_http_request_init;
    plugin_entry->apis[0].run = ls_http_request;
    plugin_entry->apis[0].terminate = ls_http_request_terminate;

    // ls_output_message
    // plugin_entry->apis[1].plugin = plugin_entry;
    // plugin_entry->apis[1].name = (char*)"ls_output_message";
    // plugin_entry->apis[1].init = ls_output_message_init;
    // plugin_entry->apis[1].run = ls_output_message;
    // plugin_entry->apis[1].terminate = ls_output_message_terminate;

    return 0;
}
