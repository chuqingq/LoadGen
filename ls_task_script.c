#include <stdlib.h>

#include <cassert>
#include <fstream>
using namespace std;

#include "jsoncpp/json/json.h"

#include "ls_utils.h"
#include "ls_task_script.h"

int load_task_script(ls_task_script_t* script) {
    LOG("==== load_task_script()\n");
    
    const char* script_file = "task/script.json";
    ifstream ifs;
    
    ifs.open(script_file);
    assert(ifs.is_open());

    Json::Reader reader;
    Json::Value root;

    assert(reader.parse(ifs, root, false));

    script->script = root;
    script->entries_num = script->script.size();
    script->entries = new ls_task_script_entry_t[script->entries_num];

    ls_task_script_entry_t* entry;
    for (size_t i = 0; i < script->entries_num; ++i)
    {
        entry = script->entries + i;

        entry->api_name = root[i]["api"].asString();
        entry->plugin_name = root[i]["plugin"].asString();
        entry->json_args = root[i]["args"];

        LOG("  api=%s, plugin_name=%s, json_args=xxx\n", entry->api_name.c_str(), entry->plugin_name.c_str());
    }

    ifs.close();// should after parsing
    return 0;
}

int unload_task_script(ls_task_script_t* script) {
    LOG("==== unload_task_script()\n");

    delete[] script->entries;
    script->entries = NULL;

    return 0;
}
