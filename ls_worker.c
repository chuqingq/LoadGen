#include <stdio.h>
#include <stdlib.h>

#include "ls_utils.h"
#include "ls_master.h"
#include "ls_worker.h"


static void worker_async_callback(uv_async_t* async, int status) {
    LOG("  worker_async_callback()\n");

    ls_worker_t* w = container_of(async, ls_worker_t, worker_async);

    if (async->data  == NULL)
    {
        LOG("ERROR ls_worker_async_cb is NULL\n");
        return;
    }

    typedef int (*ls_worker_async_cb)(ls_worker_t*);
    ls_worker_async_cb cb = ls_worker_async_cb(async->data);
    if (cb(w) < 0) {
        LOG("ERROR failed to ls_worker_async_cb()\n");
    }
}

static void worker_thread(void* arg) {
    ls_worker_t* w = (ls_worker_t*)arg;

    w->worker_loop = uv_loop_new();
    uv_async_init(w->worker_loop, &(w->worker_async), worker_async_callback);
    w->worker_started = 1;

    uv_run(w->worker_loop, UV_RUN_DEFAULT);
    LOG("  worker_thread[%u] terminate\n", (unsigned int)w->thread);
}

int worker_start(ls_worker_t* w) {
    LOG("  worker_start(%lu)\n", (unsigned long)w);

    uv_rwlock_init(&w->callmodel_delta_lock);
    w->sessions = new vector<ls_session_t*>();// TODO

    // master_async在master中初始化
    return uv_thread_create(&(w->thread), worker_thread, (void*)w);
}

// worker主动调用finish_session
int worker_stop(ls_worker_t* w) {
    LOG("  worker_stop()\n");

    // stop all sessions in the worker
    for (size_t i = 0; i < w->sessions->size(); ++i)
    {
        if (finish_session((*w->sessions)[i]) < 0) {
            LOG("ERROR failed to finish_session()\n");
            // return -1;// 确保所有session都执行了session_destroy()
        }
    }

    uv_stop(w->worker_loop);

    return 0;
}

// 在一个worker上启动num个会话
static int worker_start_new_session(ls_worker_t* w, int num) {
    LOG("  worker_start_new_session(%d)\n", num);

    ls_session_t* s;
    for (int i = 0; i < num; ++i)
    {
        s = new ls_session_t;
        s->plugin_states = (void**)malloc(master.config.plugins_num * sizeof(void*));

        // s->loop = w->worker_loop;
        s->worker = w;
        s->script = &(master.script);// 只读
        s->script_cur = -1;
        
        ls_plugin_t* plugin;
        for (size_t i = 0; i < master.num_plugins; ++i)
        {
            plugin = master.plugins + i;

            if (plugin->session_init != NULL && (plugin->session_init)(s) < 0)
            {
                LOG("ERROR failed to %s.session_init()\n", plugin->plugin_name);
                return -1;
            }
        }

        s->cur_vars = master.vars;// TODO 拷贝

        s->process = process_session;
        s->finish = finish_session;

        w->sessions->push_back(s);

        process_session(s);
    }

    return 0;
}


int worker_do_callmodel(ls_worker_t* w) {
    LOG("  worker_do_callmodel()\n");
    // worker收到master的消息
    // int delta = *((int*)async->data);
    // delete (int*)async->data;
    // async->data = NULL;
    int delta = 0;
    if (worker_get_callmodel_delta(w, &delta) < 0)
    {
        LOG("ERROR failed to worker_get_callmodel_delta()\n");
        return -1;
    }

    LOG("  worker[%lu] session_num delta=%d\n", w->thread, delta);

    /*if (delta == -1)
    {
        worker_stop(w);// TODO
        return 0;
    }
    else*/ if (delta > 0)
    {
        worker_start_new_session(w, delta);
    }

    return 0;
}

int worker_reap(ls_worker_t* w) {
    return uv_thread_join(&(w->thread));
}

int worker_set_callmodel_delta(ls_worker_t* w, int delta) {
    uv_rwlock_wrlock(&w->callmodel_delta_lock);
    w->callmodel_delta = delta;
    uv_rwlock_wrunlock(&w->callmodel_delta_lock);

    return 0;
}

int worker_get_callmodel_delta(ls_worker_t* w, int* delta) {
    uv_rwlock_rdlock(&w->callmodel_delta_lock);
    *delta = w->callmodel_delta;
    uv_rwlock_rdunlock(&w->callmodel_delta_lock);

    return 0;
}
