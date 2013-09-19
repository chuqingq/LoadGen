// 1. 返回说明此插件中的API

// load，协议加载。和任务无关
int load_plugin(ls_proto_state_t* state) {

}

int init_task(const ls_setting_t* setting, ls_proto_state_t* state) {

}

int destroy_task(ls_proto_state_t* state) {

}

// unload，协议卸载
int unload_plugin(ls_proto_state_t* state) {
    // 
}

static int plugin_demo1_api1(const JsonObj& obj, ls_proto_state_t* state, map<string, string>*vars) {

}

static LsPluginApi apis[] = {  
    {"plugin_demo1_api1", plugin_demo1_api1}  
};  

LS_PLUGIN_INIT(ls_proto_type, apis, load_plugin, unload_plugin);
