#include <stdlib.h>
#include <string.h>

#include <cassert>
#include <fstream>
using namespace std;

// #include "jsoncpp/json/json.h"
#include "lib/libjson/include/libjson.h"

#include "ls_utils.h"
#include "ls_task_script.h"

int load_task_script(ls_task_script_t* script) {
    LOG("load_task_script()\n");
    
    const char* script_file = "task/script.json";
    // ifstream ifs;
    
    // ifs.open(script_file);
    // assert(ifs.is_open());

    // Json::Reader reader;
    // Json::Value root;

    // assert(reader.parse(ifs, root, false));

    // script->script = root;
    // script->entries_num = script->script.size();
    // script->entries = new ls_task_script_entry_t[script->entries_num];

    // ls_task_script_entry_t* entry;
    // for (size_t i = 0; i < script->entries_num; ++i)
    // {
    //     entry = script->entries + i;

    //     entry->api_name = root[i]["api"].asString();
    //     entry->plugin_name = root[i]["plugin"].asString();
    //     entry->json_args = root[i]["args"];

    //     LOG("  api=%s, plugin_name=%s, json_args=xxx\n", entry->api_name.c_str(), entry->plugin_name.c_str());
    // }

    // ifs.close();// should after parsing

    // TODO 换用libjson
    char* buf;
    long len;
    FILE* f = fopen(script_file, "r");
    if (f == NULL)
    {
        LOGE("Failed to open script_file: %s\n", script_file);// TODO errno
        return -1;
    }

    fseek(f, 0, SEEK_END);
    len = ftell(f);
    rewind(f);

    buf = (char*) malloc(len + 1);
    if (buf == NULL)
    {
        LOGE("Failed to malloc for script_file\n");// TODO errno
        return -1;
    }

    len = fread(buf, 1, len, f);
    buf[len] = '\0';

    JSONNODE* root = json_parse(buf);

    script->script = root;
    script->entries_num = json_size(root);
    script->entries = new ls_task_script_entry_t[script->entries_num];

    ls_task_script_entry_t* entry;
    int ind = 0;
    for (JSONNODE_ITERATOR i = json_begin(root); i != json_end(root); ++i, ++ind)
    {
        entry = script->entries + ind;
        JSONNODE* c = *i;
        for (JSONNODE_ITERATOR j = json_begin(c); j != json_end(c); ++j)
        {
            json_char* name = json_name(*j);

            if (strcmp(name, "api") == 0)
            {
                entry->api_name = string(json_as_string(*j));
                LOG("  api: %s, ", entry->api_name.c_str());
            }
            else if (strcmp(name, "plugin") == 0)
            {
                entry->plugin_name = string(json_as_string(*j));
                LOG("  plugin: %s\n", entry->plugin_name.c_str());
            }
            else if (strcmp(name, "args") == 0)
            {
                entry->json_args = json_as_node(*j);
            }
            else
            {
                LOGE("script_file node not support: %s\n", name);
                return -1;
            }
        }
    }
    return 0;
}

int unload_task_script(ls_task_script_t* script) {
    LOG("unload_task_script()\n");

    delete[] script->entries;
    script->entries = NULL;

    return 0;
}
