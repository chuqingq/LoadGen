
## plan

HUNG 暂时不处理。有了plugin_declare()貌似就不用plugin_demo plugin_load()了
TODO The plugins need worker's notify_master() to report stats in API
TODO async interface between worker and master

TODO callmodel avg
TODO callmodel complex
TODO statistics
TODO UI
TODO master.workers改为数组，动态申请
TODO plugin从vector改为数组，states也改为数组

TODO vars
        
worker中区分只读和改动的内容，只读的内容尽量使用master的；改动的内容按session保存，plugin的个数固定

优化会话的处理：process和finish这种方式不太好

# Design

## static

    master
        config // DONE vector<string>改为char*。需要unload_config时释放
        plugins // name, 回调、api、统计项, setting. all readonly

        task_callmodel // 只有master关注

        task_setting // Json::Value.plugins_load_task_setting时调用plugin_task_init分发到plugin->settings
        task_vars // TODO 9
        task_script // plugins_load_task_script时设置api、及args（调用prepare）

    plugin[i]
        plugin_name // 1.
        callbacks  //2.
            plugin_load
            plugin_unload

            task_init
            task_destroy
    
            session_init
            session_terminate
        apis  // 3.
            api_init
            api_run
            api_destroy
        stats // 4.
        // task_setting // 不需要了，task_init/destroy中plugin自己维护
        // state // 不放在这里，放在session中，作为动态内容维护

    worker[i]
        async // 和master交互
        sessions

    session[i]
        script_cur // 只保存下标即可
        states // plugin_states. session_init/destroy
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
    
## loadgen_stats

统计模块的设计

master模块的统计
1、master加载plugin时
plugin注册：a、stats_id，b、结构void*，c、callback接收worker数据如何处理

2、master定时打印/上报统计信息
TODO 可能也要回调

3、master接收到worker的stats_id消息
调用注册的callback，来改变结构void*

plugin模块的统计

1、declare时向master注册上面三项
2、执行API时发送消息给master
