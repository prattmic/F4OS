#ifndef DEV_USBDEV_H_INCLUDED
#define DEV_USBDEV_H_INCLUDED

void init_usbdev(void) __attribute__((section(".kernel")));
void usbdev_handler(void) __attribute__((section(".kernel")));

#endif
