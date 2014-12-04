#include <stdio.h>
// #define __STDC_FORMAT_MACROS
// #include <inttypes.h>
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
    ls_session_t* session;
    map<string, uint64_t> trans;
} system_session_state_t;


typedef struct {
    ls_master_t* master;
    uv_timer_t stats_timer;// master定时2秒根据这个字段扫面trans_stats_t

    uv_mutex_t mutex;
    int64_t duration;// 平均事务响应时延
    uint64_t count;// 通过事务数
} trans_stats_t;

trans_stats_t trans_stats;


const static char* plugin_name = "ls_plugin_system";
static size_t plugin_id;

static void collect_trans_stats(uv_timer_t* timer, int status) {
    LOGP("collect_trans_stats\n");

    uv_mutex_lock(&trans_stats.mutex);
    // print
    // printf("  trans_stats: passed_trans = %"PRIu64"\n", trans_stats.count);
    cout << "  trans_stats: passed_trans = " << trans_stats.count << endl;
    if (0 == trans_stats.count) {
        printf("  trans_stats: avg_duration = N/A\n");
    } else {
        // printf("  trans_stats: avg_duration = %"PRIu64"\n", (uint64_t)trans_stats.duration/trans_stats.count);
        cout << "  trans_stats: avg_duration = " << trans_stats.duration/trans_stats.count << endl;
    }
    // clear stats
    trans_stats.count = 0;
    trans_stats.duration = 0;
    uv_mutex_unlock(&trans_stats.mutex);
}


static int ls_start_transaction_init(const JSONNODE* json_args, void** args) {
    LOGP("%s.ls_start_transaction_init()\n", plugin_name);

    string* tran_name = NULL;
    for (JSONNODE_ITERATOR i = json_begin((JSONNODE*)json_args); i != json_end((JSONNODE*)json_args); ++i) {
        json_char* name = json_name(*i);
        if (strcmp(name, "transaction_name") == 0) {
            tran_name = new string(json_as_string(*i));
        }
        json_free(name);
    }
    if (NULL == tran_name) {
        LOGP("ERROR failed to get param 'transaction_name'\n");
        return -1;
    }
    *args = tran_name;
    
    return 0;
}


static int ls_start_transaction_terminate(void** args) {
    LOGP("%s.ls_start_transaction_terminate()\n", plugin_name);
    delete((string*)(*args));
    *args = NULL;
    return 0;
}


static int ls_start_transaction(const void* args, void* sessionstate, map<string, string> * vars) {
    LOGP("%s.ls_start_transaction()\n", plugin_name);
    system_session_state_t* state = (system_session_state_t*)sessionstate;
    string* tran_name = (string*)args;

    uint64_t start = uv_now(state->session->worker->worker_loop);
    LOGP("  tran_name: %s; start: %llu\n", tran_name->c_str(), start);

    state->trans.insert(make_pair(*tran_name, start));
    process_session(state->session);
    return 0;
}


static int ls_end_transaction_init(const JSONNODE* json_args, void** args) {
    LOGP("%s.ls_end_transaction_init()\n", plugin_name);
    
    string* tran_name = NULL;
    for (JSONNODE_ITERATOR i = json_begin((JSONNODE*)json_args); i != json_end((JSONNODE*)json_args); ++i) {
        json_char* name = json_name(*i);
        if (strcmp(name, "transaction_name") == 0) {
            tran_name = new string(json_as_string(*i));
        }
        json_free(name);
    }
    if (NULL == tran_name) {
        LOGP("ERROR failed to get param 'transaction_name'\n");
        return -1;
    }
    *args = tran_name;
    return 0;
}


static int ls_end_transaction_terminate(void** args) {
    LOGP("%s.ls_end_transaction_terminate()\n", plugin_name);
    delete((string*)(*args));
    *args = NULL;
    return 0;
}


static int ls_end_transaction(const void* args, void* sessionstate, map<string, string> * vars) {
    LOGP("%s.ls_end_transaction()\n", plugin_name);

    system_session_state_t* state = (system_session_state_t*)sessionstate;
    string* tran_name = (string*)args;

    uint64_t duration = uv_now(state->session->worker->worker_loop) - state->trans[*tran_name];
    state->trans.erase(*tran_name);

    LOGP("  ls_end_transaction(): %s: %lld\n", tran_name->c_str(), duration);

    uv_mutex_lock(&trans_stats.mutex);
    trans_stats.count += 1;
    trans_stats.duration += duration;
    uv_mutex_unlock(&trans_stats.mutex);

    process_session(state->session);
    return 0;
}


static int ls_think_time_init(const JSONNODE* json_args, void** args) {
    LOGP("%s.ls_think_time_init()\n", plugin_name);

    uint64_t* time = new uint64_t;
    for (JSONNODE_ITERATOR i = json_begin((JSONNODE*)json_args); i != json_end((JSONNODE*)json_args); ++i) {
        json_char* name = json_name(*i);
        if (strcmp(name, "time") == 0) {
            *time = json_as_int(*i);
        }
        json_free(name);
    }
    if (NULL == time) {
        LOGP("ERROR failed to get param 'time'\n");
        return -1;
    }
    *args = time;
    LOGP("  time=%llu\n", *time);
    return 0;
}


static int ls_think_time_terminate(void** args) {
    LOGP("%s.ls_think_time_terminate()\n", plugin_name);
    delete((uint64_t*)*args);
    *args = NULL;
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


static int ls_think_time(const void* args, void* sessionstate, map<string, string> * vars) {
    uint64_t* time = (uint64_t*)args;
    LOGP("%s.ls_think_time(%llu)\n", plugin_name, *time);
    LOGP("  ignore_think_time=%d\n", (int)system_setting.ignore_think_time);

    system_session_state_t* state = (system_session_state_t*)sessionstate;

    uv_timer_t* timer = new uv_timer_t;// delete at line 77
    ls_worker_t* w = (ls_worker_t*)state->session->worker;
    uv_timer_init(w->worker_loop, timer);

    timer->data = state->session;
    uv_timer_start(timer, timer_cb, *time, 0);

    return 0;
}


static int ls_output_message_init(const JSONNODE* json_args, void** args) {
    LOGP("%s.ls_output_message_init()\n", plugin_name);

    string* message = NULL;
    for (JSONNODE_ITERATOR i = json_begin((JSONNODE*)json_args); i != json_end((JSONNODE*)json_args); ++i) {
        json_char* name = json_name(*i);
        if (strcmp(name, "message") == 0) {
            message = new string(json_as_string(*i));
        }
        json_free(name);
    }
    if (NULL == message) {
        LOGP("ERROR failed to get param 'message'\n");
        return -1;
    }
    *args = message;
    LOGP("  ls_output_message: %s\n", message->c_str());
    return 0;
}


static int ls_output_message_terminate(void** args) {
    LOGP("%s.ls_output_message_terminate()\n", plugin_name);
    delete((string*)(*args));
    *args = NULL;
    return 0;
}


static int ls_output_message(const void* args, void* sessionstate, map<string, string> * vars) {
    system_session_state_t* state = (system_session_state_t*)sessionstate;
    printf("  ls_output_message: %s\n", ((string*)args)->c_str());
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
    // TODO 1. 读取setting到本地的结构

    // 2. 读注册统计回调
    if (uv_timer_init(m->master_loop, &trans_stats.stats_timer) < 0) {
        LOGE("trans_stats_timer init error\n");
        return -1;
    }
    trans_stats.master = m;

    if (uv_timer_start(&trans_stats.stats_timer, collect_trans_stats, 100, 1000) < 0) {
        LOGE("trans_stats_timer start error\n");
        return -1;
    }
    
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

    system_session_state_t* state = (system_session_state_t*)malloc(sizeof(system_session_state_t));
    if (state == NULL)
    {
        LOGP("ERROR failed to malloc system_session_state\n");
        return -1;
    }
    state->trans = map<string, uint64_t>();
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
    plugin_entry->num_apis = 4;
    plugin_entry->apis = (ls_plugin_api_t*)malloc(plugin_entry->num_apis * sizeof(ls_plugin_api_t));

    // ls_think_time
    plugin_entry->apis[0].plugin = plugin_entry;
    plugin_entry->apis[0].name = (char*)"ls_think_time";
    plugin_entry->apis[0].init = ls_think_time_init;
    plugin_entry->apis[0].run = ls_think_time;
    plugin_entry->apis[0].terminate = ls_think_time_terminate;

    // ls_output_message
    plugin_entry->apis[1].plugin = plugin_entry;
    plugin_entry->apis[1].name = (char*)"ls_output_message";
    plugin_entry->apis[1].init = ls_output_message_init;
    plugin_entry->apis[1].run = ls_output_message;
    plugin_entry->apis[1].terminate = ls_output_message_terminate;

    // ls_start_transaction
    plugin_entry->apis[2].plugin = plugin_entry;
    plugin_entry->apis[2].name = (char*)"ls_start_transaction";
    plugin_entry->apis[2].init = ls_start_transaction_init;
    plugin_entry->apis[2].run = ls_start_transaction;
    plugin_entry->apis[2].terminate = ls_start_transaction_terminate;

    // ls_end_transaction
    plugin_entry->apis[3].plugin = plugin_entry;
    plugin_entry->apis[3].name = (char*)"ls_end_transaction";
    plugin_entry->apis[3].init = ls_end_transaction_init;
    plugin_entry->apis[3].run = ls_end_transaction;
    plugin_entry->apis[3].terminate = ls_end_transaction_terminate;

    return 0;
}
