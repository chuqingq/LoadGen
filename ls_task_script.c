#include <cassert>
#include <fstream>
using namespace std;

#include "jsoncpp/json/json.h"

#include "ls_task_script.h"

int load_task_script(ls_task_script_t* script) {
    printf("==== enter load_task_script()\n");
    const char* script_file = "task/script.json";
    ifstream ifs;
    
    ifs.open(script_file);
    assert(ifs.is_open());

    Json::Reader reader;
    Json::Value root;

    assert(reader.parse(ifs, root, false));

    printf("script.size() = %d\n", root.size());

    for (int i = 0; i < root.size(); ++i)
    {
        ls_task_script_entry_t entry;

        entry.api_name = root[i]["api"].asString();
        entry.plugin_name = root[i]["plugin"].asString();
        entry.args_json = root[i]["args"];// 拷贝一份

        script->push_back(entry);
    }

    ifs.close();

    printf("root[0][\"api\"] = %s\n", root[0]["api"].asString().c_str());

    return 0;
}
