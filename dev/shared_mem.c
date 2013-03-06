#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <mm/mm.h>
#include <kernel/semaphore.h>
#include <kernel/sched.h>
#include <kernel/fault.h>
#include <mm/mm.h>
#include <dev/resource.h>

#include <dev/shared_mem.h>

#define SM_SIZE   CONFIG_SHARED_MEM_SIZE

typedef struct shared_mem { 
        char        data[SM_SIZE]; 
        int         read_ctr; 
        int         write_ctr; 
} shared_mem; 

char shared_mem_read(void *env, int *error) __attribute__((section(".kernel")));
int shared_mem_write(char c, void *env) __attribute__((section(".kernel")));
int shared_mem_close(resource *env) __attribute__((section(".kernel")));

rd_t open_shared_mem(void) {
    shared_mem *mem = kmalloc(sizeof(shared_mem));
    if (!mem) {
        printk("OOPS: Could not allocate memory for shared memory resource.\r\n");
        return -1;
    }

    resource *new_r = create_new_resource();
    if (!new_r) {
        printk("OOPS: Unable to allocate memory for shared memory resource.\r\n");
        kfree(mem);
        return -1;
    }

    mem->read_ctr = 0;
    mem->write_ctr = 0;
    memset(mem->data, '\0', SM_SIZE);

    new_r->env = mem;
    new_r->writer = &shared_mem_write;
    new_r->swriter = NULL;
    new_r->reader = &shared_mem_read;
    new_r->closer = &shared_mem_close;
    new_r->read_sem = kmalloc(sizeof(semaphore));
    if (new_r->read_sem) {
        init_semaphore(new_r->read_sem);
    }
    else {
        printk("OOPS: Unable to allocate memory for shared memory resource.\r\n");
        kfree(new_r);
        kfree(mem);
        return -1;
    }
    new_r->write_sem = new_r->read_sem;

    return add_resource(curr_task->task, new_r);
}

char shared_mem_read(void *env, int *error) {
    if (error != NULL) {
        *error = 0;
    }

    shared_mem *mem = (shared_mem *)env;
    if(mem->read_ctr > 512) {
        mem->read_ctr = 1;
        return mem->data[0];
    }
    else return mem->data[mem->read_ctr++];
}

int shared_mem_write(char c, void *env) {
    shared_mem *mem = (shared_mem *)env;
    if(mem->write_ctr > 512) {
        mem->write_ctr = 1;
        mem->data[0] = c;
    }
    else mem->data[mem->write_ctr++] = c;

    return 1;
}

int shared_mem_close(resource *resource) {
    kfree(resource->env);
    acquire_for_free(resource->read_sem);
    kfree((void*) resource->read_sem);

    return 0;
}
