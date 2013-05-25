typedef struct command {
    char *name;
    void (*fptr)(int, char **);
} command_t;

#define DEFINE_APP(fn) command_t fn##_command __attribute__((section(".commands")))= {\
        .name = #fn,        \
        .fptr = fn          \
    };
