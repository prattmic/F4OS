#ifndef DEV_USBDEV_H_INCLUDED
#define DEV_USBDEV_H_INCLUDED

void init_usbdev(void) __attribute__((section(".kernel")));
void usbdev_handler(void) __attribute__((section(".kernel")));

struct semaphore;
extern struct semaphore usbdev_semaphore;

struct resource;

void usbdev_resource_write(char c, void *env);
char usbdev_resource_read(void *env);
void usbdev_resource_close(struct resource *resource);

#endif
