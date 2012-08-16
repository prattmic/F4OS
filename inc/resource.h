
extern resource default_resources[RESOURCE_TABLE_SIZE];

void add_resource(task_ctrl* tcs, resource* r) __attribute__((section(".kernel")));
void resource_setup(task_ctrl* tcs) __attribute__((section(".kernel")));
void write(rd_t rd, char *d, int n) __attribute__((section(".kernel")));
void swrite(rd_t rd, char *s) __attribute__((section(".kernel")));
void read(rd_t rd, char *buf, int n) __attribute__((section(".kernel")));
