#ifndef LS_TASK_VAR_H_
#define LS_TASK_VAR_H_

#include <string>
#include <map>
using namespace std;

typedef enum {
    VAR_UNIQUE = 0,
    VAR_RANDOM,
    VAR_FILE
} ls_task_var_type;

typedef struct {
    string var_name;
    ls_task_var_type var_type;
    // TODO
} ls_task_var_entry_t;

typedef map<string, ls_task_var_entry_t> ls_task_var_t;// 变量，master读取，master不需要，worker只读

int load_task_vars(ls_task_var_t* vars) {
    // TODO 可能还要根据type分类
}

#endif
