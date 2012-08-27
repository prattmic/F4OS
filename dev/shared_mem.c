#include <stdlib.h>
#include <mm/mm.h>
#include <kernel/semaphore.h>
#include <kernel/sched.h>
#include <dev/resource.h>

#include <dev/shared_mem.h>

#define SM_SIZE   512

typedef struct shared_mem { 
        char        data[SM_SIZE]; 
        int         read_ctr; 
        int         write_ctr; 
} shared_mem; 

char shared_mem_read(void *env) __attribute__((section(".kernel")));
void shared_mem_write(char c, void *env) __attribute__((section(".kernel")));
void shared_mem_close(resource *env) __attribute__((section(".kernel")));

rd_t open_shared_mem(void) {
    shared_mem *mem = kmalloc(sizeof(shared_mem));
    resource *new_r = kmalloc(sizeof(resource));

    mem->read_ctr = 0;
    mem->write_ctr = 0;

    new_r->env = mem;
    new_r->writer = &shared_mem_write;
    new_r->reader = &shared_mem_read;
    new_r->closer = &shared_mem_close;
    new_r->sem    = kmalloc(sizeof(semaphore));
    init_semaphore(new_r->sem);

    add_resource(curr_task->task, new_r);

    return curr_task->task->top_rd - 1;
}

char shared_mem_read(void *env) {
    shared_mem *mem = (shared_mem *)env;
    if(mem->read_ctr > 512) {
        mem->read_ctr = 1;
        return mem->data[0];
    }
    else return mem->data[mem->read_ctr++];
}

void shared_mem_write(char c, void *env) {
    shared_mem *mem = (shared_mem *)env;
    if(mem->write_ctr > 512) {
        mem->write_ctr = 1;
        mem->data[0] = c;
    }
    else mem->data[mem->write_ctr++] = c;
}

void shared_mem_close(resource *resource) {
    kfree(resource->env);
    acquire(resource->sem);
    kfree((void*) resource->sem);
}
