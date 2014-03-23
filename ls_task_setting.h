#ifndef LS_TASK_SETTING_H_
#define LS_TASK_SETTING_H_

#include <string>
#include <map>
using namespace std;

// #include "jsoncpp/json/json.h"
#include "lib/libjson/include/libjson.h"

// TODO change to Json::Value ?? 
// typedef map<string/* plugin_type */, Json::Value* /* 最初是Json::Value，后续是协议自己的内容 */> ls_task_setting_t;
// typedef Json::Value ls_task_setting_t;
typedef JSONNODE* ls_task_setting_t;// TODO 需要修改函数实现

int load_task_setting(ls_task_setting_t* setting);
int unload_task_setting(ls_task_setting_t* setting);

#endif
