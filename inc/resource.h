void add_resource(task_ctrl* tcs, resource* r) __attribute__((section(".kernel")));
void resource_setup(task_ctrl* tcs) __attribute__((section(".kernel")));
void write(rd_t rd, char* s) __attribute__((section(".kernel")));
