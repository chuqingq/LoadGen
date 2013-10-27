#include <cassert>
#include <fstream>
using namespace std;

#include "jsoncpp/json/json.h"

#include "ls_utils.h"
#include "ls_task_setting.h"

int load_task_setting(ls_task_setting_t* setting) {
    LOG("==== load_task_setting()\n");
    const char* setting_file = "task/setting.json";
    ifstream ifs;
    
    ifs.open(setting_file);
    assert(ifs.is_open());

    Json::Reader reader;
    Json::Value root;

    assert(reader.parse(ifs, root, false));
    ifs.close();

    *setting = root;
    // Json::Value::Members m = root.getMemberNames();
    // for (Json::Value::Members::iterator it = m.begin(); it != m.end(); ++it)
    // {
    //     LOG("  setting of plugin = %s\n", it->c_str());

    //     Json::Value *settings = new Json::Value();
    //     *settings = root[*it];

    //     setting->insert(pair<string, Json::Value*>(*it, settings));
    // }

    return 0;
}

int unload_task_setting(ls_task_setting_t* setting) {
    // TODO not called
    return -1;

}
