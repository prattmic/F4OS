#ifndef DEV_RESOURCE_H_INCLUDED
#define DEV_RESOURCE_H_INCLUDED

#include <stdint.h>

typedef uint8_t rd_t;

struct semaphore;

typedef struct resource {
    volatile struct semaphore   *sem;
    void                        *env;
    void                        (*writer)(char, void*);
    char                        (*reader)(void*);
    void                        (*closer)(struct resource*);
} resource;

struct task_ctrl;

void add_resource(struct task_ctrl* tcs, resource* r) __attribute__((section(".kernel")));
void resource_setup(struct task_ctrl* tcs) __attribute__((section(".kernel")));

#endif
