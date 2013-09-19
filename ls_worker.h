
typedef ls_session_s {
    // 只读
    ls_script_t* script;
    ls_runtimesetting_t* settings;
    ls_data_t* data;

    ls_worker_t* worker;// 所属的worker
    // 脚本当前执行到哪里了
    ls_session_script_t* cur_script;
    ls_session_rtsetting_t* cur_settigs;
    ls_session_data_t* cur_data;
} ls_session_t;


typedef ls_worker_s {
    uv_thread_t thread;
    uv_loop_t* worker_loop;

    uv_async_t worker_async;/* master向worker通信 */
    uv_async_t master_async;/* worker向worker通信 */

    // 多个session
    vector<ls_session_t*> sessions;
} ls_worker_t;

int process_session(ls_session_t* session) {
    //
}
