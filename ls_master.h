
// 1. master
typedef struct {
    uv_loop_t* master_loop;

    ls_config_t* config;// DONE loadgen设置，maste读取，后续只读，worker不使用
    ls_plugin_t* plugins;// TODO 保存加载的插件

    ls_callmodel_t* callmodel;// 呼叫模型，master保存，master使用
    map<string, void*> settings;// proto_type -> task_setting master读取setting.json并调用plugin的plugin_load_setting解析，后续master不用，worker只读
    map<string, ls_var_t> vars;// 变量，master读取，master不需要，worker只读
    ls_script_t* script;// 脚本，master保存，worker只读
    
    vector<ls_worker_t*> workers;
} ls_master_t;


typedef struct {
    int session_id;// 暂时无用

    const map<string, void*> *settings;// 引用master的 TODO ？？ 可有可无

    ls_script_t cur_script;// session拷贝一份执行逻辑，需要标记走到哪一步了

    map<string, void*> states;
    map<string, ls_var_t> cur_vars;// const map<string, ls_var_t> vars;// session需要根据此初始化变量cur_vars、每次执行API前获取需要的变量更新cur_vars
} ls_session_t;


typedef struct {
    uv_loop_t* worker_loop;

    uv_async_t master_async;
    uv_async_t worker_async;

    vector<ls_session_t> sessions;

} ls_worker_t;
