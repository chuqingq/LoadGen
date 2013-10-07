#include <stdio.h>

#include <string>
#include <map>
#include <iostream>
using namespace std;

#include "ls_plugin.h"
#include "ls_worker.h"
#include "ls_session.h"

typedef struct {
    bool ignore_think_time;
} ls_plugin_demo_setting_t;

ls_plugin_demo_setting_t plugin_demo_setting;

typedef struct {
    // TODO
} ls_plugin_demo_state_t;

ls_plugin_demo_state_t plugin_demo_state;

// load，协议加载。和任务无关
static int plugin_load() {
    printf("  >>>> plugin_demo plugin_load()\n");
    return 0;
}

static int plugin_unload() {
    printf("  >>>> plugin_demo plugin_unload()\n");
    return 0;
}

static int plugin_task_init(const Json::Value* setting) {
    printf("  >>>> plugin_demo plugin_task_init()\n");

    plugin_demo_setting.ignore_think_time = (*setting)["ignore_think_time"].asBool();
    return 0;
}

static int plugin_task_destroy() {
    printf("  >>>> plugin_demo plugin_task_destroy()\n");
    
    return 0;
}

static int plugin_session_init(void** state) {
    printf("  >>>> plugin_demo plugin_session_init()\n");

    return 0;
}

static int plugin_session_destroy(void** state) {
    printf("  >>>> plugin_demo plugin_session_destroy()\n");

    return 0;
}

// ls_think_time
static int ls_think_time_init(const Json::Value* json_args, void** args) {
    printf("  >>>> plugin_demo ls_think_time init()\n");
    // TODO
    *args = (void*)json_args;
    return 0;
}

static int ls_think_time_destroy(void** args) {
    printf("  >>>> plugin_demo ls_think_time destroy()\n");

    return 0;
}

static void timer_cb(uv_timer_t* handle, int status) {
    printf("  >>>> plugin_demo ls_think_time timer_cb()\n");
    // 根据handle获取到session
    ls_session_t* s = (ls_session_t*) handle->data;
    uv_timer_stop(handle);
    delete handle;

    (s->process)(s);// TODO process_session处理下一个api
}

static int ls_think_time(const void* args, ls_session_t* session, map<string, string> * vars) {
    printf("  >>>> plugin_demo ls_think_time(%d)\n", 1);

    printf("  ignore_think_time=%d\n", (int)plugin_demo_setting.ignore_think_time);

    uv_timer_t* timer = new uv_timer_t;// TODO 貌似应该动态申请
    uv_timer_init(session->loop, timer);

    timer->data = session;
    uv_timer_start(timer, timer_cb, 1000, 0);// TODO 时间是写死的

    return 0;
}

// ls_error_message
static int ls_error_message_init(const Json::Value* json_args, void** args) {
    printf("  >>>> plugin_demo ls_error_message init()\n");
    // TODO
    *args = (void*)json_args;
    return 0;
}

static int ls_error_message_destroy(void** args) {
    printf("  >>>> plugin_demo ls_error_message destroy()\n");

    return 0;
}

static int ls_error_message(const void* args, ls_session_t* session, map<string, string> * vars) {
    printf("  >>>> plugin_demo ls_error_message(%d)\n", 1);

    return 0;
}

extern "C" int plugin_declare(const char** plugin_name, ls_plugin_entry_t* plugin_entry) {
    printf("  >>>> plugin_declare(%s)\n", (char*) plugin_name);

    *plugin_name = "ls_plugin_demo";

    plugin_entry->plugin_load = &plugin_load;
    plugin_entry->plugin_unload = &plugin_unload;

    plugin_entry->task_init = plugin_task_init;
    plugin_entry->task_destroy = plugin_task_destroy;

    plugin_entry->session_init = plugin_session_init;
    plugin_entry->session_destroy = plugin_session_destroy;

    ls_plugin_api_entry_t api_entry;

    // ls_think_time
    api_entry.init = ls_think_time_init;
    api_entry.api = ls_think_time;
    api_entry.destroy = ls_think_time_destroy;
    plugin_entry->apis.insert(pair<string, ls_plugin_api_entry_t>("ls_think_time", api_entry));


    // ls_error_message
    api_entry.init = ls_error_message_init;
    api_entry.api = ls_error_message;
    api_entry.destroy = ls_error_message_destroy;
    plugin_entry->apis.insert(pair<string, ls_plugin_api_entry_t>("ls_error_message", api_entry));

    return 0;
}
