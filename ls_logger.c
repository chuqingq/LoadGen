#include "ls_logger.h"

// TODO add static before ??
FILE* _loadgen_log = NULL;
FILE* _plugin_log = NULL;

int log_init() {
    const char* loadgen_log_path = "loadgen.log";
    const char* plugin_log_path = "service.log";

    _loadgen_log = fopen(loadgen_log_path, "a+");
    if (loadgen_log_path == NULL)
    {
        printf("ERROR failed to fopen loadgen_log_path\n");
        return -1;
    }
    setlinebuf(_loadgen_log);

    _plugin_log = fopen(plugin_log_path, "a+");
    if (_plugin_log == NULL)
    {
        printf("ERROR failed to fopen plugin_log_path\n");
        return -1;
    }
    setlinebuf(_plugin_log);

    return 0;
}

int log_terminate() {
    if (fclose(_loadgen_log) < 0)
    {
        printf("ERROR failed to fclose _loadgen_log\n");
    }

    if (fclose(_plugin_log) < 0)
    {
        printf("ERROR failed to fclose _plugin_log\n");
    }

    return 0;
}

