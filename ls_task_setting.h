#ifndef LS_TASK_SETTING_H_
#define LS_TASK_SETTING_H_

#include <string>
#include <map>
using namespace std;

#define JsonObj void

typedef map<string/* plugin_type */, void* /* 最初是JsonObj，后续是协议自己的内容 */> ls_task_setting_t;

int load_task_setting(ls_task_setting_t* setting);

#endif