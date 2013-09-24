#include <cassert>
#include <fstream>
using namespace std;

#include "jsoncpp/json/json.h"

#include "ls_task_setting.h"

int load_task_setting(ls_task_setting_t* setting) {
    printf("==== enter load_task_setting()\n");
    const char* setting_file = "task/setting.json";
    ifstream ifs;
    
    ifs.open(setting_file);
    assert(ifs.is_open());

    Json::Reader reader;
    Json::Value root;

    assert(reader.parse(ifs, root, false));
    ifs.close();

    printf("setting.size() = %d\n", root.size());

    Json::Value::Members m = root.getMemberNames();
    for (Json::Value::Members::iterator it = m.begin(); it != m.end(); ++it)
    {
        Json::Value *settings = new Json::Value();
        *settings = root[*it];

        setting->insert(pair<string, void*>(*it, settings));
    }    

    return 0;
}
