// map<string, void*> settings; // proto_type -> proto_setting

// JsonObj -> 协议相关的setting
// 只需在task_init中把本协议相关的setting传入插件的回调即可，插件自行保存到自己的state中