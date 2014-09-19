#include "ls_config.h"

#include <stdlib.h>
#include <string.h>

#include <errno.h>

#include "lib/libjson/include/libjson.h"
#include "ls_utils.h"

int load_config(ls_config_t* config) {
    LOG("load_config()\n");

    const char* config_file = "config.json";

    char* buf;
    long len;
    FILE* f = fopen(config_file, "r");
    if (NULL == f) {
        LOGE("Failed to fopen config_file[%s]: %s\n", config_file, strerror(errno));
        return -1;
    }

    fseek(f, 0, SEEK_END);
    len = ftell(f);
    rewind(f);

    buf = (char*) malloc(len + 1);
    if (NULL == buf) {
        LOGE("Failed to malloc for config_file: %s\n", strerror(errno));
        return -1;
    }

    len = fread(buf, 1, len, f);
    buf[len] = '\0';

    JSONNODE* n = json_parse(buf);

    for (JSONNODE_ITERATOR i = json_begin(n); i != json_end(n); ++i) {
        json_char* name = json_name(*i);

        if (strcmp(name, "workers_num") == 0) {
            config->workers_num = json_as_int(*i);
        }

        json_free(name);
    }

    return 0;
}

int unload_config(ls_config_t* config) {
    LOG("unload_config()\n");

    config->workers_num = 0;

    return 0;
}
