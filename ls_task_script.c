#include <cassert>
#include <fstream>
using namespace std;

#include "jsoncpp/json/json.h"

#include "ls_task_script.h"

int load_task_script(ls_task_script_t* script) {
    printf("==== load_task_script()\n");
    const char* script_file = "task/script.json";
    ifstream ifs;
    
    ifs.open(script_file);
    assert(ifs.is_open());

    Json::Reader reader;
    Json::Value root;

    assert(reader.parse(ifs, root, false));

    for (size_t i = 0; i < root.size(); ++i)
    {
        ls_task_script_entry_t entry;

        entry.api_name = root[i]["api"].asString();
        entry.plugin_name = root[i]["plugin"].asString();
        entry.json_args = root[i]["args"];// 拷贝一份

        script->push_back(entry);

        printf("  api=%s, plugin_name=%s, json_args=xxx\n", entry.api_name.c_str(), entry.plugin_name.c_str());
    }

    ifs.close();
    return 0;
}
