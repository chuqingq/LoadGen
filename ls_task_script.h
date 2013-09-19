
// script

// TODO 先不要prepare的动作

// ls_script_entry_t script.json中的一项
typedef struct {
    // 前两项是从script.json中直接读出来的
    string api_name;
    string proto_name;
    JsonObj* args_json;// TODO 使用jsoncpp解析script.json

    // 这两项是parse之后的结果
    ls_plugin_api_t* api;// 包含API的名称、函数指针 TODO master要有读取plugin的动作
    ls_plugin_api_args_t* args;// prepare时从args_json转换来 TODO 暂时不做prepare，因此ls_plugin_api_args_t就是jsonobj
} ls_script_entry_t;

typedef vector<ls_script_entry_t> ls_script_t;

int read_script(ls_script_t* script);
