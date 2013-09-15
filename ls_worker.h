
typedef ls_worker_s {
    uv_thread_t thread;
    uv_loop_t* worker_loop;

    uv_async_t worker_async;/* master向worker通信 */
    uv_async_t master_async;/* worker向worker通信 */
} ls_worker_t;

