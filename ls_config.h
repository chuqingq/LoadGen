#ifndef LS_CONFIG_H_
#define LS_CONFIG_H_

#include <string>
#include <vector>
using namespace std;

typedef struct {
    int worker_num;
    vector<string> plugin_paths;
} ls_config_t;

int load_config(ls_config_t* config);
int unload_config(ls_config_t* config);

#endif
