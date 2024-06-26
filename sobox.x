typedef string str_t<255>;

struct dlopen_args {
    str_t filename;
    int flags;
};

struct dlsym_args {
    uint64_t handle;
    str_t symbol;
};

struct dlcall_args {
    uint64_t symbol;
    uint64_t args[6];
};

program SOBOX {
    version SOBOX_VERSION {
        uint64_t SBXDLOPEN(dlopen_args) = 1;
        uint64_t SBXDLSYM(dlsym_args) = 2;
        uint64_t SBXDLCALL(dlcall_args) = 3;
    } = 1;
} = 1000;
