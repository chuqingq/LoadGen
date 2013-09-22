#include "ls_task_setting.h"

int load_task_setting(ls_task_setting_t* setting) {
    // TODO
    map<string, string> * plugin_demo = new map<string, string>();
    plugin_demo->insert(pair<string, string>("setting_name1", "setting_value1"));
    plugin_demo->insert(pair<string, string>("setting_name2", "setting_value2"));

    setting->insert(pair<string, void*>("ls_plugin_demo", plugin_demo));

    // map<string, string> * plugin_demo2 = new map<string, string>();
    // plugin_demo2->insert(pair<string, string>("setting_name3", "setting_value3"));
    // plugin_demo2->insert(pair<string, string>("setting_name4", "setting_value4"));

    // setting->insert(pair<string, void*>("ls_plugin_demo2", plugin_demo2));

    return 0;
}
