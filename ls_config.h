
typedef struct ls_config_s {
    short port;
    int worker_num;
} ls_config_t;

int read_config(ls_config_t* config);
