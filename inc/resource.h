typedef struct pipe {
    char *addr;
    uint32_t reader_pid;
    uint32_t writer_pid;
} pipe;

typedef struct pipe_node {
    struct pipe_node *next;
    pipe *curr;
    struct pipe_node *prev;
} pipe_node;

typedef struct pipe_list {
    pipe_node *head;
    pipe_node *tail;
} pipe_list;

void add_resource(task_ctrl* tcs, resource* r) __attribute__((section(".kernel")));
void resource_setup(task_ctrl* tcs) __attribute__((section(".kernel")));
void write(rd_t rd, char* s) __attribute__((section(".kernel")));
pipe create_pipe(uint32_t reader_pid, uint32_t writer_pid) __attribute__((section(".kernel")));
void add_pipe_to_list(pipe_node* p) __attribute__((section(".kernel")));
