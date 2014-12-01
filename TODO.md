
## plan

TODO statistics
TODO system插件已经能够统计事务，但是master要怎么计算并处理？？
    需要system在master_init中在master注册回调：每秒获取所有worker的数据（需要加锁）
    并“上报”（如何上报）
    事务统计项：当前一秒：通过失败个数
        各个事务响应时延：{TransName, Pass, Duration}
    TODO wrk是如何实现的? 多个worker线程给master上报数据?

HUNG 暂时不处理。有了plugin_declare()貌似就不用plugin_demo plugin_load()了
TODO The plugins need worker's notify_master() to report stats in API
TODO async interface between worker and master // master和worker互相通知时data设置为接口int async_cb(ls_worker_t*)

TODO callmodel avg
TODO callmodel complex

TODO 脚本中如何支持for/while/if这种逻辑分支和循环

TODO 集合点ls_rendevous
    master的state做变量，达到目标后。。。

TODO 在util中重定义malloc和free，检查是否有内存泄漏

TODO front-UI
TODO vars
        
worker中区分只读和改动的内容，只读的内容尽量使用master的；改动的内容按session保存，plugin的个数固定

优化会话的处理：process和finish这种方式不太好

# Design

## static

    master
        config // config of master, 'config.json'. load_config/unload_config. 
                // TODO 去掉plugin信息，加载task脚本时再加载plugin
        plugins // name, callback, api, stats, setting // TODO 只保留相关的，加载后应该就不用了

        task_callmodel // master关注。只支持主叫（线形组合）和被叫（被拉起session）

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
    plugin_load_task // 让plugin对task进行预处理
        setting // void**
        script // api, args
        vars
    start_workers
        sessions
            task_script_cur // index
            plugin_states
            vars
    stop_workers // TODO 未调用worker_init
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

## TODO plugin模块的统计
1、declare时向master注册上面三项
2、执行API时发送消息给master

## TODO
去掉config，监听端口等内容后续放在system插件中
去掉log，放在utils中
