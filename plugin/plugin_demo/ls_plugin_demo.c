#include <stdio.h>

#include <string>
#include <map>
using namespace std;

#include "ls_plugin.h"
#include "ls_worker.h"
#include "ls_session.h"


// load，协议加载。和任务无关
static int plugin_load() {
    printf(">>>> plugin_demo plugin_load()\n");
    return 0;
}

static int plugin_unload() {
    printf(">>>> plugin_demo plugin_unload()\n");
    return 0;
}

static int plugin_task_init(const void* setting, void** plugin_setting, void** plugin_state) {
    printf(">>>> plugin_demo plugin_task_init()\n");
    // TODO setting  -> plugin_setting
    // new plugin_state
    return 0;
}

static int plugin_task_destroy(void** plugin_setting, void** plugin_state) {
    printf(">>>> plugin_demo plugin_task_destroy()\n");
    // delete plugin_setting and plugin_state
    return -1;
}

static void timer_cb(uv_timer_t* handle, int status) {
    printf(">>>> ls_think_time timer_cb()\n");
    // 根据handle获取到session
    ls_session_t* s = (ls_session_t*) handle->data;
    uv_timer_stop(handle);
    delete handle;

    (s->process)(s);// TODO process_session处理下一个api
}

static int ls_think_time_prepare(const Json::Value* json_args, void** args) {
    printf(">>>> ls_think_time_prepare()\n");
    // TODO
    *args = (void*)json_args;
    return 0;
}

// static int ls_think_time(uv_loop_t* loop, const void* args, void* plugin_state, map<string, string> * vars) {
static int ls_think_time(const void* args, ls_session_t* session, map<string, string> * vars) {
    printf(">>>> plugin_demo before ls_think_time(%d)\n", 1);

    uv_timer_t* timer = new uv_timer_t;// TODO 貌似应该动态申请
    uv_timer_init(session->loop, timer);

    timer->data = session;
    uv_timer_start(timer, timer_cb, 1000, 0);// TODO 时间是写死的

    return 0;
}


static int ls_error_message_prepare(const Json::Value* json_args, void** args) {
    printf(">>>> ls_error_message_prepare()\n");
    // TODO
    *args = (void*)json_args;
    return 0;
}

static int ls_error_message(const void* args, ls_session_t* session, map<string, string> * vars) {
    printf(">>>> plugin_demo before ls_error_message(%d)\n", 1);

    return 0;
}

extern "C" int plugin_declare(const char** plugin_name, ls_plugin_entry_t* plugin_entry) {
    printf(">>>> plugin_declare(%s)\n", plugin_name);

    *plugin_name = "ls_plugin_demo";

    plugin_entry->plugin_load = &plugin_load;
    plugin_entry->plugin_unload = &plugin_unload;

    plugin_entry->task_init = plugin_task_init;
    plugin_entry->task_destroy = plugin_task_destroy;

    ls_plugin_api_entry_t api_entry;

    // ls_think_time
    api_entry.prepare = ls_think_time_prepare;
    api_entry.api = ls_think_time;
    plugin_entry->apis.insert(pair<string, ls_plugin_api_entry_t>("ls_think_time", api_entry));


    // ls_error_message
    api_entry.prepare = ls_error_message_prepare;
    api_entry.api = ls_error_message;
    plugin_entry->apis.insert(pair<string, ls_plugin_api_entry_t>("ls_error_message", api_entry));

    return 0;
}
