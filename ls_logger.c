#include "ls_logger.h"

#include <string.h>
#include <errno.h>

FILE* _loadgen_log = NULL;
FILE* _plugin_log = NULL;

int log_init() {
    const char* loadgen_log_path = "loadgen.log";

    _loadgen_log = fopen(loadgen_log_path, "a+");
    if (NULL == _loadgen_log) {
        fprintf(stderr, "ERROR failed to fopen loadgen log[%s]: %s\n",
            loadgen_log_path, strerror(errno));
        return -1;
    }
    setlinebuf(_loadgen_log);

    const char* plugin_log_path = "service.log";

    _plugin_log = fopen(plugin_log_path, "a+");
    if (NULL == _plugin_log) {
        fprintf(stderr, "ERROR failed to fopen service log[%s]: %s\n",
            plugin_log_path, strerror(errno));
        return -1;
    }
    setlinebuf(_plugin_log);

    return 0;
}

int log_terminate() {
    fclose(_loadgen_log);
    _loadgen_log = NULL;

    fclose(_plugin_log);
    _plugin_log = NULL;
    
    return 0;
}
