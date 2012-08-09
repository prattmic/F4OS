#define SP_SIZE   512

typedef struct shared_page {
    semaphore   sem;
    char        data[SP_SIZE];    
} shared_page;

extern resource default_resources[RESOURCE_TABLE_SIZE];

void add_resource(task_ctrl* tcs, resource* r) __attribute__((section(".kernel")));
void resource_setup(task_ctrl* tcs) __attribute__((section(".kernel")));
void write(rd_t rd, char* s) __attribute__((section(".kernel")));
char read(rd_t rd) __attribute__((section(".kernel")));
rd_t open_shared_page(task_node bro_task) __attribute__((section(".kernel")));
