#include "ls_task_var.h"

#include "ls_logger.h"

int load_task_vars(ls_task_var_t* vars) {
    LOG("load_task_vars()\n");
    ls_task_var_entry_t var;
    var.var_name = "var_test";
    var.var_type = VAR_UNIQUE;

    vars->insert(pair<string, ls_task_var_entry_t>("var_test", var));

    // TODO 可能还要根据type分类
    return 0;
}

int unload_task_vars(ls_task_var_t* vars) {
    LOG("unload_task_vars()\n");

    // TODO

    return 0;
}
