#include <cassert>
#include <stdlib.h>
#include <string.h>

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

    // allocate plugin_paths
    config->plugin_paths = (char**) malloc((config->worker_num + 1) * sizeof(char*));
    if (config->plugin_paths == NULL)
    {
        printf("ERROR failed to malloc plugin_paths.\n");
        return -1;
    }

    // plugins_num and plugin_paths
    Json::Value plugin_paths = root["plugin_paths"];
    config->plugins_num = plugin_paths.size();
    size_t i;
    for (i = 0; i < config->plugins_num; ++i)
    {
        printf("  plugin_paths[%d] = %s\n", i, plugin_paths[i].asString().c_str());
        // config->plugin_paths.push_back(plugin_paths[i].asString());
        string path = plugin_paths[i].asString();
        int len = path.length();

        char* path_dst = (char*) malloc(len + 1);
        memcpy(path_dst, path.c_str(), len + 1);
        config->plugin_paths[i] = path_dst;
    }
    config->plugin_paths[i] = NULL;

    ifs.close();
    return 0;
}

int unload_config(ls_config_t* config) {
    printf("==== unload_config()\n");
    
    for (size_t i = 0; i < config->plugins_num; ++i)
    {
        free(config->plugin_paths[i]);
        config->plugin_paths[i] = NULL;
    }
    free(config->plugin_paths);
    config->plugin_paths = NULL;
    config->plugins_num = 0;

    return 0;
}