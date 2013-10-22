#include "ls_stats.h"

int handle_stats(void* once_data, ls_stats_entry_t* entry) {
    if ((entry->handler)(once_data, entry->state) < 0)
    {
        printf("ERROR failed to handle_stats()\n");
        return -1;
    }

    return 0;
}

int output_stats(ls_stats_entry_t* stats, size_t entries_num) {
    ls_stats_entry_t* entry;
    for (size_t i = 0; i < entries_num; ++i)
    {
        entry = stats + i;
        if ((entry->output)(entry->state) < 0)
        {
            printf("ERROR failed to output_stats(%d)\n", i);
            // return -1;// make sure do other outputs
        }
    }

    return 0;
}
