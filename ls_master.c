#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>

#include "ls_master.h"
#include "ls_worker.h"

ls_master_t master;

int load_plugins(ls_master_t* master) {
    printf("==== load_plugins()\n");

    master->num_plugins = master->config.plugins_num;

    // allocate master->plugins
    master->plugins = (ls_plugin_t*)malloc(master->num_plugins * sizeof(ls_plugin_t));

    ls_plugin_t* plugin;
    for (size_t i = 0; i < master->num_plugins; ++i)
    {
        plugin = master->plugins + i;
        plugin->plugin_index = i;

        if (uv_dlopen(master->config.plugin_paths[i], &(plugin->plugin_lib)) < 0) {
            printf("  Failed to uv_dlopen\n");
            return -1;
        }

        if (uv_dlsym(&(plugin->plugin_lib), "plugin_declare", (void**)&(plugin->plugin_declare)) < 0) {
            printf("  Failed to uv_dlsym\n");
            return -1;
        }

        if ((plugin->plugin_declare(plugin) < 0)) {
            printf("  Failed to plugin_declare\n");
            return -1;
        }

        if ((plugin->master_init)(master) < 0)
        {
            printf("ERROR failed to plugin_load()\n");
            return -1;
        }
    }

    return 0;
}

int unload_plugins(ls_master_t* master) {
    printf("==== unload_plugins()\n");

    ls_plugin_t* plugin;
    for (size_t i = 0; i < master->num_plugins; ++i)
    {
        plugin = master->plugins + i;
        printf("  plugin=%s\n", plugin->plugin_name);

        // 1. 调用plugin_unload()
        if ((plugin->master_terminate)(master) < 0)
        {
            printf("ERROR failed to plugin_unload()\n");
            // return -1;// 确保所有插件都正常卸载
        }
        uv_dlclose(&plugin->plugin_lib);
    }

    free(master->plugins);
    master->plugins = NULL;
    master->num_plugins = 0;

    return 0;
}

typedef void (*ls_master_async_cb)(void* data);

void master_async_stats_handle(void* data) {
    // handle_stats(xxx, yyy); // TODO
}

static void master_async_callback(uv_async_t* handle, int status) {
    printf("  master_async_callback()\n");

    ls_master_notify_type type = *(ls_master_notify_type*)(handle->data);

    
    switch (type) {
        case NOTIFY_STATS:
        {
            ls_stats_notify_t* stats_notify = (ls_stats_notify_t*)handle->data;
            if (handle_stats(stats_notify->data, stats_notify->entry) < 0) {
                printf("ERROR failed to handle_stats()\n");
            }
        }
        break;
        default:
            printf("  unknown master async\n");
            break;
    }

    free(handle->data);
    handle->data = NULL;
}

int start_workers(ls_master_t* master) {
    printf("==== start_workers()\n");

    master->num_workers = master->config.workers_num;
    master->workers = (ls_worker_t*)malloc(master->num_workers * sizeof(ls_worker_t));
    if (master->workers == NULL)
    {
        printf("ERROR failed to malloc workers\n");
        return -1;
    }

    for (size_t i = 0; i < master->num_workers; ++i)
    {
        ls_worker_t* w = master->workers + i;
        w->worker_started = 0;

        if (uv_async_init(master->master_loop, &(w->master_async), master_async_callback) < 0) {
            printf("ERROR failed to uv_async_init(master)\n");
            return -1;
        }

        if (init_worker(w) < 0)// start worker thread
        {
            printf("ERROR failed to init_worker()\n");
            return -1;
        }

        while (w->worker_started == 0) ;
    }

    return 0;
}

int stop_workers(ls_master_t* master) {
    printf("==== stop_workers()\n");

    for (size_t i = 0; i < master->num_workers; ++i)
    {
        ls_worker_t* w = master->workers + i;

        if (worker_set_callmodel_delta(w, -1) < 0)
        {
            printf("ERROR failed to worker_set_callmodel_delta()\n");
            return -1;
        }

        if (uv_async_send(&(w->worker_async)) < 0)
        {
            printf("ERROR failed to uv_async_send()\n");
            return -1;
        }
    }

    return 0;
}

int reap_workers(ls_master_t* master) {
    printf("==== reap_workers()\n");

    for (size_t i = 0; i< master->num_workers; ++i)
    {
        printf("  before reap_workers(%d)\n", i);

        if (reap_worker(master->workers + i) < 0) {
            printf("ERROR failed to read_worker(%d)\n", i);
            return -1;
        }
    }

    return 0;
}

static int notify_worker_start_new_session(ls_worker_t* w, int num) {
    printf("  ==== notify_worker_start_new_session(%lu, %d)\n", (unsigned long)w, num);

    if (worker_set_callmodel_delta(w, num) < 0)
    {
        printf("ERROR failed to worker_set_callmodel_delta()\n");
        return -1;
    }

    w->worker_async.data = (void*)worker_do_callmodel;

    printf("  before master uv_async_send()\n");
    return uv_async_send(&(w->worker_async));
}

int start_new_session(int num) {
    printf("  ==== start_new_session(%d)\n", num);
    
    // 先按简单的方式来：num尽量平均分给每个worker，不考虑worker当前的会话数
    int workers_num = master.num_workers;
    int avg = num/workers_num + 1;
    int add = num%workers_num;
    printf("  avg=%d,add=%d\n", avg, add);

    for (int i = 0; i < add; ++i)
    {
        if (notify_worker_start_new_session(master.workers + i, avg) < 0)
        {
            printf("  Failed to worker_start_new_session()\n");
            return -1;
        }
    }

    if ((avg - 1) == 0)
    {
        return 0;
    }

    for (int i = add; i < workers_num; ++i)
    {
        if (notify_worker_start_new_session(master.workers + i, avg-1) < 0)
        {
            printf("  Failed to worker_start_new_session()\n");
            return -1;
        }
    }

    return 0;
}

// // 遍历master的所有plugin，执行他们的统计output
// static void do_stats_per_sec(uv_timer_t* handle, int status) {
//     printf("  do_stats_per_sec()\n");

//     ls_plugin_t* plugin;
//     for (size_t i = 0; i < master.num_plugins; ++i)
//     {
//         plugin = master.plugins + i;

//         ls_stats_entry_t* stats_entry;
//         for (size_t i = 0; i < plugin->stats_num; ++i)
//         {
//             stats_entry = plugin->stats + i;

//             if ((stats_entry->output)(stats_entry->state) < 0)
//             {
//                 printf("ERROR failed to stats_entry->output()\n");
//                 // make sure do all output
//             }
//         }
//     }
// }


int start_stats() {
    // uv_timer_t* t = &master.stats_timer;
    // uv_timer_init(master.master_loop, t);
    // uv_timer_start(t, do_stats_per_sec, 1000, 1000);

    return 0;
}
