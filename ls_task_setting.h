#ifndef LS_TASK_SETTING_H_
#define LS_TASK_SETTING_H_

#include <string>
#include <map>
using namespace std;

#define JsonObj void

typedef map<string/* plugin_type */, JsonObj*> ls_task_setting_t;

int load_task_setting(ls_task_setting_t* setting);

#endif
