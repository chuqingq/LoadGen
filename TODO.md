
## DONE

HUNG 暂时不处理。有了plugin_declare()貌似就不用plugin_demo plugin_load()了
DONE plugins_load_task_setting // TODO 具体实现

DONE plugins_unload_task_setting/script

DONE task_init/destroy中只传入Json::Value setting，自己的setting和state自行维护
    OK 去掉entry中的plugin_setting
    TODO 去掉entry中的plugin_state：！！！需要在session_init/destroy中补充
DONE session_init/session_terminate
    DONE plugin中增加这两个回调接口
    DONE plugin_demo中增加实现
    DONE worker_start_new_session中调用session_init
        finish_session中调用session_tereminate

DONE 没有走到plugin_session_destroy
    需要worker停掉自己的所有session

DONE 确保unload_plugins调用plugin_unload()

DONE 一个api有3个操作init/run/destory，master在plugins_load_task_script时执行init，master在plugins_unload_task_script时执行destroy
    plugins_unload_task_script增加第一个script参数

DONE recompile libuv, and then compile loadgen and run test

## TODO

DONE make sure master async_send after worker_thread async_init
  current: sleep(3)
  make a worker_started for each worker, set it after async_init
TODO callmodel avg
TODO callmodel complex
TODO statistics
TODO UI
TODO master.workers改为数组，动态申请
TODO plugin从vector改为数组，states也改为数组
        
worker中区分只读和改动的内容，只读的内容尽量使用master的；改动的内容按session保存，plugin的个数固定

优化会话的处理：process和finish这种方式不太好

# Design

## static

    master
        config // DONE vector<string>改为char*。需要unload_config时释放
        plugins // 回调、api、状态

        task_callmodel // 只有master关注

        task_setting // plugins_load_task_setting时调用分发到plugin的task_setting中
        task_vars // TODO 9
        task_script // plugins_load_task_script时设置api、及args（调用prepare）

    plugin[i]    // TODO 2
        plugin_name
  
        callbacks
            plugin_load
            plugin_unload

            task_init
            task_destroy
    
            session_init
            session_terminate
        apis
            api_init
            api_run
            api_destroy

        task_setting // TODO 不需要了，task_init/destroy中plugin自己维护
        state // TODO 不放在这里，放在session中，作为动态内容维护

    worker[i]
        async // 和master交互
        sessions

    session[i]
        // 只读，直接使用master的task_script、plugins等
        cur_task_script // TODO 只保存下标即可
        states // TODO map<string/* plugin_name */, void*> session_init/destroy维护
        cur_task_vars // TODO 9

## dynamic

    load_config
    load_plugins // plugin_unload
    load_task
        setting // Json::Value
        script // Array
        vars // TODO
    plugin_load_task
        setting // void**
        script // api, args
        vars
    start_workers
        sessions
            task_script_cur // index
            plugin_states
            vars
    stop_workers
    plugin_unload_task
    unload_task
    unload_plugins
    unload_config
    