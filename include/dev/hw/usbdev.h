#ifndef DEV_USBDEV_H_INCLUDED
#define DEV_USBDEV_H_INCLUDED

void init_usbdev(void) __attribute__((section(".kernel")));
int usbdev_tx(uint8_t endpoint, uint32_t *packet, int size) __attribute__((section(".kernel")));
void usbdev_rx(uint32_t *buf, int bytes) __attribute__((section(".kernel")));
void usbdev_handler(void) __attribute__((section(".kernel")));

#endif
