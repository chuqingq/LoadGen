#include <stdlib.h>
#include <cassert>
#include <fstream>
using namespace std;

// #include "jsoncpp/json/json.h"
#include "lib/libjson/include/libjson.h"

#include "ls_utils.h"
#include "ls_task_setting.h"

int load_task_setting(ls_task_setting_t* setting) {
    LOG("load_task_setting()\n");
    const char* setting_file = "task/setting.json";
    // ifstream ifs;
    
    // ifs.open(setting_file);
    // assert(ifs.is_open());

    // Json::Reader reader;
    // Json::Value root;

    // assert(reader.parse(ifs, root, false));
    // ifs.close();

    // *setting = root;
    // Json::Value::Members m = root.getMemberNames();
    // for (Json::Value::Members::iterator it = m.begin(); it != m.end(); ++it)
    // {
    //     LOG("  setting of plugin = %s\n", it->c_str());

    //     Json::Value *settings = new Json::Value();
    //     *settings = root[*it];

    //     setting->insert(pair<string, Json::Value*>(*it, settings));
    // }

    char* buf;
    long len;
    FILE* f = fopen(setting_file, "r");
    if (f == NULL)
    {
        printf("Failed to open setting_file: %s\n", setting_file);// TODO errno
        return -1;
    }

    fseek(f, 0, SEEK_END);
    len = ftell(f);
    rewind(f);

    buf = (char*) malloc(len + 1);
    if (buf == NULL)
    {
        printf("Failed to malloc for setting_file\n");// TODO errno
        return -1;
    }

    len = fread(buf, 1, len, f);
    buf[len] = '\0';

    *setting = json_parse(buf);
    return 0;
}

int unload_task_setting(ls_task_setting_t* setting) {
    // TODO not called
    return -1;

}
