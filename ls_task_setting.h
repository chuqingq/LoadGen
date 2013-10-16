#ifndef LS_TASK_SETTING_H_
#define LS_TASK_SETTING_H_

#include <string>
#include <map>
using namespace std;

#include "jsoncpp/json/json.h"

// TODO change to Json::Value ?? 
typedef map<string/* plugin_type */, Json::Value* /* 最初是Json::Value，后续是协议自己的内容 */> ls_task_setting_t;

int load_task_setting(ls_task_setting_t* setting);

#endif
