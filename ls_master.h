

typedef ls_master_s {
    uv_loop_t* master_loop;
    ls_config_t* config;// loadgen设置，maste读取，后续只读，worker不使用

    vector< map<string, map<string, string> > > script;// 脚本，master保存，worker只读
    vector< map<string, map<string, string> > > setting;// master读取，但master不需要，worker需要
    map<string, ls_data_t*> vars;// 变量，master读取，master不需要，worker只读
    ls_callmodel_t* callmodel;// 呼叫模型，master保存，master使用

    map<string, map<string, LsPluginApi> > plugins;// TODO 保存加载的插件
    vector<ls_worker_t*> workers;
} ls_master_t;


typedef ls_session_s {
    int session_id;

    const XXX script;
    const XXX setting;
    const XXX vars;

    YYY cur_script;
    // setting
    YYY cur_vars;
} ls_session_t;


typedef ls_worker_s {
    uv_loop_t* worker_loop;

    uv_async_t master_async;
    uv_async_t worker_async;

    vector<ls_session_t*> sessions;

} ls_worker_t;