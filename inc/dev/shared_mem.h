#define SM_SIZE   512

typedef struct shared_mem { 
        char        data[SM_SIZE]; 
        int         read_ctr; 
        int         write_ctr; 
} shared_mem; 

rd_t open_shared_mem(void) __attribute__((section(".kernel")));
char shared_mem_read(void *env) __attribute__((section(".kernel")));
void shared_mem_write(char c, void *env) __attribute__((section(".kernel")));
