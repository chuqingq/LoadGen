#pragma once

#include <stdio.h>
#include <stddef.h>

#define container_of(ptr, type, member) ((type *)((char *) (ptr) - offsetof(type, member)))

int log_init();
int log_terminate();

extern FILE* _loadgen_log;
extern FILE* _plugin_log;

// #define LOG(fmt, args...) fprintf(_loadgen_log, fmt, ##args)
// #define LOG(fmt, args...) printf("INFO "fmt, ##args)
#define LOG(fmt, args...) {}
// #define LOGE(fmt, args...) fprintf(_loadgen_log, fmt, ##args)
#define LOGE(fmt, args...) printf("ERROR "fmt, ##args)
// #define LOGP(fmt, args...) fprintf(_plugin_log, fmt, ##args)
// #define LOGP(fmt, args...) printf(fmt, ##args)
#define LOGP(fmt, args...) {}

