#include <cassert>
#include <stdlib.h>
#include <string.h>

#include <fstream>
using namespace std;

#include "jsoncpp/json/json.h"

#include "ls_utils.h"
#include "ls_config.h"

int load_config(ls_config_t* config) {
    LOG("load_config()\n");

    const char* config_file = "config.json";
    // ifstream ifs;
    
    // ifs.open(config_file);
    // assert(ifs.is_open());

    // Json::Reader reader;
    // Json::Value root;

    // assert(reader.parse(ifs, root, false));

    // // workers_num
    // config->workers_num = root["workers_num"].asInt();
    // LOG("  workers_num = %d\n", config->workers_num);

    // // allocate plugin_paths
    // config->plugin_paths = (char**) malloc((config->workers_num + 1) * sizeof(char*));
    // if (config->plugin_paths == NULL)
    // {
    //     LOG("ERROR failed to malloc plugin_paths.\n");
    //     return -1;
    // }

    // // plugins_num and plugin_paths
    // Json::Value plugin_paths = root["plugin_paths"];
    // config->plugins_num = plugin_paths.size();
    // size_t i;
    // for (i = 0; i < config->plugins_num; ++i)
    // {
    //     LOG("  plugin_paths[%d] = %s\n", i, plugin_paths[i].asString().c_str());
        
    //     string path = plugin_paths[i].asString();
    //     int len = path.length();

    //     char* path_dst = (char*) malloc(len + 1);
    //     memcpy(path_dst, path.c_str(), len + 1);
    //     config->plugin_paths[i] = path_dst;
    // }
    // config->plugin_paths[i] = NULL;

    // ifs.close();

    char* buf;
    long len;
    FILE* f = fopen(config_file, "r");
    if (f == NULL)
    {
        printf("Failed to open config_file: %s\n", config_file);// TODO errno
        return -1;
    }

    fseek(f, 0, SEEK_END);
    len = ftell(f);
    rewind(f);

    buf = (char*) malloc(len + 1);
    if (buf == NULL)
    {
        printf("Failed to malloc for config_file\n");// TODO errno
        return -1;
    }

    len = fread(buf, 1, len, f);
    buf[len] = '\0';

    JSONNODE* n = json_parse(buf);

    // workers_num

    // plugins_num and plugin_paths

    for (JSONNODE_ITERATOR i = json_begin(n); i != json_end(n); ++i)
    {
        json_char* name = json_name(*i);

        if (strcmp(name, "workers_num") == 0)
        {
            config->worker_num = json_as_int(*i);
        }
        else if (strcmp(name, "plugin_paths") == 0)
        {
            JSONNODE* plugin_paths = json_as_array(*i);
            config->plugins_num = json_size(plugin_paths);
            config->plugin_paths = (char**) malloc(sizeof(char*) * config->plugins_num);
            // for (JSONNODE_ITERATOR j = json_begin(plugin_paths); j != json_end(plugin_paths); ++j)
            for (size_t j = 0; j < config->plugins_num; ++j)
            {
                JSONNODE *p = json_at(*i, j);
                json_char* path = json_as_string(p);
                config->plugin_paths[j] = (char*) malloc(sizeof(char*) * (strlen(path) + 1));
                memcpy(config->plugin_paths[j], path, strlen(path) + 1);
                json_free(path);
            }
        }

        json_free(name);
    }

    return 0;
}

int unload_config(ls_config_t* config) {
    LOG("()\n");
    
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
