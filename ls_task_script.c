#include "ls_task_script.h"

#include <stdlib.h>
#include <string.h>

#include <cassert>
#include <fstream>
using namespace std;

#include <errno.h>

#include "lib/libjson/include/libjson.h"

#include "ls_utils.h"
#include "ls_master.h"

int load_task_script(ls_task_script_t* script) {
    LOG("load_task_script()\n");
    
    const char* script_file = "task/script.json";
    char* buf;
    long len;
    FILE* f = fopen(script_file, "r");
    if (f == NULL)
    {
        LOGE("Failed to open script_file[%s]: %s\n", script_file, strerror(errno));
        return -1;
    }

    fseek(f, 0, SEEK_END);
    len = ftell(f);
    rewind(f);

    buf = (char*) malloc(len + 1);
    assert(NULL != buf);

    len = fread(buf, 1, len, f);
    buf[len] = '\0';

    JSONNODE* root = json_parse(buf);

    script->entries_num = json_size(root);
    script->entries = new ls_task_script_entry_t[script->entries_num];// TODO

    ls_task_script_entry_t* entry;
    int ind = 0;
    for (JSONNODE_ITERATOR i = json_begin(root); i != json_end(root); ++i, ++ind) {
        entry = script->entries + ind;
        JSONNODE* c = *i;
        for (JSONNODE_ITERATOR j = json_begin(c); j != json_end(c); ++j) {
            json_char* name = json_name(*j);
            if (strcmp(name, "api") == 0) {
                const char* api_name = json_as_string(*j);
                entry->api = find_api_by_name(api_name, master.plugins, master.num_plugins);
                if (NULL == entry->api) {
                    LOGE("  api %s not found\n", api_name);
                    return -1;
                }
                printf("  api=%s\n", entry->api->name);
            }
            else if (strcmp(name, "args") == 0) {
                JSONNODE* args = json_as_node(*j);
                if ((entry->api->init)(args, &entry->args) < 0) {
                    LOGE("  api %s.init() error\n", entry->api->name);
                    return -1;
                }
            }
            else {
                LOGE("script_file node not support: %s\n", name);
                return -1;
            }
            json_free(name);// TODO 上面return会有内存泄露
        }
    }
    return 0;
}

int unload_task_script(ls_task_script_t* script) {
    LOG("unload_task_script()\n");

    for (size_t i = 0; i < script->entries_num; ++i)
    {
        ls_task_script_entry_t* entry = script->entries + i;
        if (entry->api->terminate(&entry->args) < 0) {
            LOGE("  api %s.terminate() error\n", entry->api->name);
        }
    }
    
    delete[] script->entries;
    script->entries = NULL;
    script->entries_num = 0;

    return 0;
}
