
typedef struct {
    string var_name;
    ls_var_type var_type;
    // TODO
} ls_var_entry_t;

typedef map<string, ls_var_entry_t> ls_var_t;// 变量，master读取，master不需要，worker只读

int load_task_var(ls_var_t* vars) {
    // TODO 可能还要根据type分类
}