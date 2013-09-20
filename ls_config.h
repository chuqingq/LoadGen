#ifndef LS_CONFIG_H_
#define LS_CONFIG_H_


#include <string>
using namespace std;

typedef struct {
    // short port;// TODO 暂时不用
    // string cur_dir;// 当前路径。加载plugin、读取task脚本等都需要
    int worker_num;
} ls_config_t;

extern ls_config_t config;

int load_config(ls_config_t* config);

#endif
