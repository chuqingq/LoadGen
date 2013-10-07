#include <stdio.h>

#include "ls_utils.h"
#include "ls_master.h"
#include "ls_worker.h"

static int worker_stop_all_sessions(ls_worker_t* w) {
    printf("==== worker_stop_all_sessions()\n");

    for (size_t i = 0; i < w->sessions.size(); ++i)
    {
        if (finish_session(w->sessions[i]) < 0) {
            printf("ERROR failed to finish_session()\n");
            // return -1;// 确保所有session都执行了session_destroy()
        }
    }

    return 0;
}

static void worker_async_callback(uv_async_t* async, int status) {
    printf("==== worker_async_callback()\n");

    ls_worker_t* w = container_of(async, ls_worker_t, worker_async);
    // worker收到master的消息
    // int delta = *((int*)async->data);
    // delete (int*)async->data;
    // async->data = NULL;
    int delta = 0;
    if (worker_get_callmodel_delta(w, &delta) < 0)
    {
        printf("ERROR failed to worker_get_callmodel_delta()\n");
        return;
    }

    printf("  worker[%lu] session_num delta=%d\n", w->thread, delta);

    if (delta == -1)
    {
        // worker停止自己的所有session
        if (worker_stop_all_sessions(w) < 0) {
            printf("ERROR failed to worker_stop_all_sessions()\n");
        }

        // -1表示master要停止worker
        uv_stop(w->worker_loop);
        return;
    }

    if (delta > 0)
    {
        worker_start_new_session(w, delta);
    }
}

static void worker_thread(void* arg) {
    ls_worker_t* w = (ls_worker_t*)arg;

    w->worker_loop = uv_loop_new();
    uv_async_init(w->worker_loop, &(w->worker_async), worker_async_callback);
    // printf("  worker_thread() thread:%d, loop:%d\n", w->thread, w->worker_loop);

    uv_run(w->worker_loop, UV_RUN_DEFAULT);
    printf("==== worker_thread() thread terminate\n");
}

int init_worker(ls_worker_t* w) {
    printf("  init_worker()\n");
    // master_async在master中初始化
    return uv_thread_create(&(w->thread), worker_thread, (void*)w);
}

int reap_worker(ls_worker_t* w) {
    return uv_thread_join(&(w->thread));
}

// 在一个worker上启动num个会话
int worker_start_new_session(ls_worker_t* w, int num) {
    printf("==== worker_start_new_session(%d)\n", num);

    ls_session_t* s;

    for (int i = 0; i < num; ++i)
    {
        s = new ls_session_t;

        s->session_id = 0;// TODO
        s->loop = w->worker_loop;
        // printf("  worker_start_new_session() thread:%d, loop:%d\n", w->thread, s->loop);
        // s->settings = &(master.settings);// 只读
        s->script = &(master.script);// 只读
        s->script_cur = -1;
        // state从master的plugins中state保存过来
        for (map<string, ls_plugin_entry_t>::iterator it = master.plugins.begin(); it != master.plugins.end(); ++it)
        {
            ls_plugin_entry_t* e = &(it->second);

            void* state = NULL;
            if ((e->session_init)(&state) < 0)// TODO
            {
                printf("ERROR failed to session_init()\n");
                return -1;
            }

            s->states.insert(pair<string, void*>(it->first, state));// TODO 改为数组
        }

        s->cur_vars = master.vars;// TODO 拷贝

        s->process = process_session;
        s->finish = finish_session;

        w->sessions.push_back(s);
        process_session(s);
    }

    return 0;
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
