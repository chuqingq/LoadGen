#include <stdio.h>

#include <string>
#include <map>
using namespace std;

#include "../../ls_plugin.h"


// load，协议加载。和任务无关
static int plugin_load() {
    printf("plugin_demo plugin_load()\n");
    return 0;
}

static int plugin_unload() {
    printf("plugin_demo plugin_unload()\n");
    return 0;
}

static int plugin_task_init(const void* setting, void** plugin_setting, void** plugin_state) {
    printf("plugin_demo plugin_task_init()\n");
    // TODO setting  -> plugin_setting
    // new plugin_state
    return 0;
}

static int plugin_task_destroy(void** plugin_setting, void** plugin_state) {
    printf("plugin_demo plugin_task_destroy()\n");
    // delete plugin_setting and plugin_state
    return -1;
}

static int plugin_demo1_api1(const void* args, void* plugin_state, map<string, string> * vars) {
    printf("plugin_demo plugin_demo1_api1: %d\n", 1);
    return 0;
}

extern "C" int plugin_declare(const char** plugin_name, ls_plugin_entry_t* plugin_entry) {
    *plugin_name = "ls_plugin_demo";

    plugin_entry->plugin_load = &plugin_load;
    plugin_entry->plugin_unload = &plugin_unload;

    plugin_entry->task_init = plugin_task_init;
    plugin_entry->task_destroy = plugin_task_destroy;

    plugin_entry->apis.insert(pair<string, ls_plugin_api_t>("plugin_demo1_api1", plugin_demo1_api1));

    printf("plugin_declare() task_init=%d\n", plugin_task_init);
    return 0;
}
