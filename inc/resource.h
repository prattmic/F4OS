#define SM_SIZE   512

typedef struct shared_mem {
    char        data[SM_SIZE];
    int         read_ctr;
    int         write_ctr;
} shared_mem;

extern resource default_resources[RESOURCE_TABLE_SIZE];

void add_resource(task_ctrl* tcs, resource* r) __attribute__((section(".kernel")));
void resource_setup(task_ctrl* tcs) __attribute__((section(".kernel")));
void write(rd_t rd, char *d, int n) __attribute__((section(".kernel")));
void swrite(rd_t rd, char *s) __attribute__((section(".kernel")));
void read(rd_t rd, char *buf, int n) __attribute__((section(".kernel")));
rd_t open_shared_mem(void) __attribute__((section(".kernel")));
char shared_mem_read(void *env) __attribute__((section(".kernel")));
void shared_mem_write(char c, void *env) __attribute__((section(".kernel")));
