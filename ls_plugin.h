


// ls_api_t 函数指针
typedef int ls_api_t(ls_plugin_api_args_t*, void* proto_state);

typedef struct {
    ls_plugin_load_t* plugin_load;// master启动时调用
    ls_plugin_unload_t* plugin_unload;// master退出前调用

    ls_plugin_task_init_t* task_init;// master在任务启动前调用。加载setting，脚本参数预处理？？ TODO
    ls_plugin_task_destroy_t* task_destroy;// master在任务结束后调用。卸载setting
    
    map<string, ls_plugin_api_t*> apis;

} ls_plugin_entry_t;

typedef map<string, ls_plugin_entry_t> ls_plugin_t;// first: plugin_name

int load_plugins(ls_plugin_t* plugins) {
}

int unload_plugins(ls_plugin_t* plugins) {
}

// 把master加载的任务设置，分协议加载，转换成自己的类型
int plugins_load_task_settings(const map<string, JsonObj*> & settings,
                               ls_plugin_t* plugins) {
}

int plugins_unload_task_settings(ls_plugin_t* plugins) {
}
