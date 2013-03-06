#ifndef DEV_RESOURCE_H_INCLUDED
#define DEV_RESOURCE_H_INCLUDED

#include <stdint.h>

typedef int8_t rd_t;

struct semaphore;

typedef struct resource {
    /* Separate read and write semaphores available.
     * For resources that require one lock for all
     * actions, these should be set equal to one another. */
    volatile struct semaphore   *read_sem;
    volatile struct semaphore   *write_sem;
    void                        *env;
    int                         (*writer)(char, void*);
    int                         (*swriter)(char*, void*);   /* Optional string writer function used by swrite if available  */
    char                        (*reader)(void*, int*);
    int                         (*closer)(struct resource*);
} resource;

struct task_ctrl;

resource *create_new_resource(void) __attribute__((section(".kernel")));
rd_t add_resource(struct task_ctrl* tcs, resource* r) __attribute__((section(".kernel")));
void resource_setup(struct task_ctrl* tcs) __attribute__((section(".kernel")));

extern resource *default_resources[];

#endif
