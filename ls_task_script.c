#include "ls_task_script.h"

int load_task_script(ls_task_script_t* script) {// TODO 需要根据plugin来对script进行处理
    ls_task_script_entry_t entry;

    entry.api_name = "plugin_demo1_api1";
    entry.plugin_name = "";// TODO 
    entry.args_json = NULL;
    entry.api = NULL;// TODO
    entry.args = NULL;

    script->push_back(entry);

    return -1;
}
