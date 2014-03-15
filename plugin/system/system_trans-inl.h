#ifndef LS_SYSTEM_TRANS_H_
#define LS_SYSTEM_TRANS_H_

#include "ls_utils.h"

// 事物统计 in worker
// ....

typedef {
    uint64_t count;
    uint64_t total;
} tran_t;
typedef map<string, tran_t> system_trans_t;

// static int system_trans_init() {
//     return -1;
// }

// static int system_trans_terminate() {

// }

static int system_trans_add(system_trans_t* worker_trans, const char* trans_name, int trans_flag, uint64_t trans_duration) {
    // TODO
    system_trans_t::iterator it = worker_trans->find(trans_name);
    if (it != worker_trans->end())
    {
        printf("ERROR sss\n");
        return -1;
    }

    it->second.count++;
    it->second.total += trans_duration;

    printf("  avg = %llu\n", it->second.total/it->second.count);

    return 0;
}

// ls_start_transaction
static int ls_start_transaction_init(const Json::Value* json_args, void** args) {
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

    Json::Value* json_args = (Json::Value*)args;
    string name = (*json_args)["transaction_name"].asString();

    uint64_t start = uv_now(session->worker->worker_loop);
    LOGP("  start: %llu\n", start);

    system_session_state_t* state = (system_session_state_t*)session->plugin_states[plugin_id];
    state->trans.insert(make_pair(name, start));

    process_session(session);
    return 0;
}

// ls_end_transaction
static int ls_end_transaction_init(const Json::Value* json_args, void** args) {
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

    Json::Value* json_args = (Json::Value*)args;
    string name = (*json_args)["transaction_name"].asString();

    uint64_t stop = uv_now(session->worker->worker_loop);

    system_session_state_t* state = (system_session_state_t*)session->plugin_states[plugin_id];
    uint64_t start = state->trans[name];
    LOGP("  start:%llu, stop:%llu\n", start, stop);
    state->trans.erase(name);

    LOGP("  ls_end_transaction(): %s: %lld\n", name.c_str(), stop-start);
    // TODO 向worker统计
    ls_worker_t* w = session->worker;
    system_worker_state_t* workerstate = w->plugin_states[plugin_id];


    process_session(session);
    return 0;
}

#endif
