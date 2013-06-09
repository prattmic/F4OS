#ifndef DEV_RESOURCE_H_INCLUDED
#define DEV_RESOURCE_H_INCLUDED

#include <stdint.h>

struct task_t;
typedef struct task_t task_t;

#define RESOURCE_TABLE_SIZE         CONFIG_RESOURCE_TABLE_SIZE

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

struct task_resource_data {
    struct resource *resources[RESOURCE_TABLE_SIZE];
    rd_t top_rd;
};

resource *create_new_resource(void) __attribute__((section(".kernel")));
rd_t add_resource(task_t *task, resource* r) __attribute__((section(".kernel")));
void task_resource_setup(task_t *task) __attribute__((section(".kernel")));

extern resource *default_resources[];

/* Determine default stdout/stderr */
extern struct resource usart_resource;
extern struct resource usb_resource;

#define USART_DEV   &usart_resource
#define USB_DEV     &usb_resource

#if defined(CONFIG_STDOUT_USART)
#define STDOUT_DEV        USART_DEV
#elif defined(CONFIG_STDOUT_USB)
#define STDOUT_DEV        USB_DEV
#else
#define STDOUT_DEV        NULL
#endif

#if defined(CONFIG_STDERR_USART)
#define STDERR_DEV        USART_DEV
#elif defined(CONFIG_STDERR_USB)
#define STDERR_DEV        USB_DEV
#else
#define STDERR_DEV        NULL
#endif

#endif
