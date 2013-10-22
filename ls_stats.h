#ifndef LS_STATS_H_
#define LS_STATS_H_

#include <stdio.h>

// #include "ls_master.h"
// enum ls_master_notify_type;
// typedef enum ls_master_notify_type {
//     NOTIFY_STATS = 0,
//     NOTIFY_OTHER
// } ls_master_notify_type;


typedef int (*ls_stats_handler_t)(void* one_data, void* stats_state);
typedef int (*ls_stats_output_t)(void* stats_state);

typedef struct {
    void* state;
    ls_stats_handler_t handler;// master how to handle worker's once_data
    ls_stats_output_t output;// master how to print out
} ls_stats_entry_t;

typedef struct {
    int type;
    void* data;
    ls_stats_entry_t* entry;
} ls_stats_notify_t;

int handle_stats(void* once_data, ls_stats_entry_t* entry) ;
int output_stats(ls_stats_entry_t* entry, size_t entries_num);

#endif