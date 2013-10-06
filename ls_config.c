#include <cassert>
#include <fstream>
using namespace std;

#include "jsoncpp/json/json.h"

#include "ls_config.h"

int load_config(ls_config_t* config) {
    printf("==== load_config()\n");

    const char* config_file = "config.json";
    ifstream ifs;
    
    ifs.open(config_file);
    assert(ifs.is_open());

    Json::Reader reader;
    Json::Value root;

    assert(reader.parse(ifs, root, false));

    // worker_num
    config->worker_num = root["worker_num"].asInt();
    printf("  worker_num = %d\n", config->worker_num);

    // plugin_paths
    Json::Value plugin_paths = root["plugin_paths"];
    for (size_t i = 0; i < plugin_paths.size(); ++i)
    {
        printf("  plugin_paths[%d] = %s\n", i, plugin_paths[i].asString().c_str());
        config->plugin_paths.push_back(plugin_paths[i].asString());
    }
    
    ifs.close();
    return 0;
}
