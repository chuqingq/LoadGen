#ifndef LS_LOGGER_H_
#define LS_LOGGER_H_

#include <stdio.h>

int log_init();
int log_terminate();

extern FILE* _loadgen_log;
extern FILE* _plugin_log;

#define LOG(fmt, args...) fprintf(_loadgen_log, fmt, ##args)
// #define LOGP(fmt, args...) fprintf(_plugin_log, fmt, ##args)
#define LOGP(fmt, args...) printf(fmt, ##args)

#endif
