#ifndef DEV_USBDEV_H_INCLUDED
#define DEV_USBDEV_H_INCLUDED

/* SETUP Packet */
#define     USB_SETUP_REQUEST_TYPE_RECIPIENT(r)             (r & 0xF)
#define     USB_SETUP_REQUEST_TYPE_RECIPIENT_DEVICE         (0)
#define     USB_SETUP_REQUEST_TYPE_RECIPIENT_INTERFACE      (1)
#define     USB_SETUP_REQUEST_TYPE_RECIPIENT_ENDPOINT       (2)
#define     USB_SETUP_REQUEST_TYPE_RECIPIENT_OTHER          (3)

#define     USB_SETUP_REQUEST_TYPE_TYPE(r)                  ((r & 0xF0) >> 4)
#define     USB_SETUP_REQUEST_TYPE_TYPE_STD                 (0)
#define     USB_SETUP_REQUEST_TYPE_TYPE_CLASS               (1)
#define     USB_SETUP_REQUEST_TYPE_TYPE_VENDOR              (2)

#define     USB_SETUP_REQUEST_TYPE_DIR(r)                   ((r & (1 << 7)) >> 7)
#define     USB_SETUP_REQUEST_TYPE_DIR_H2D                  (0)
#define     USB_SETUP_REQUEST_TYPE_DIR_D3H                  (1)

#define     USB_SETUP_REQUEST_CLEAR_FEATURE                 (1)
#define     USB_SETUP_REQUEST_GET_CONFIGURATION             (8)
#define     USB_SETUP_REQUEST_GET_DESCRIPTOR                (6)
#define     USB_SETUP_REQUEST_GET_INTERFACE                 (10)
#define     USB_SETUP_REQUEST_GET_STATUS                    (0)
#define     USB_SETUP_REQUEST_SET_ADDRESS                   (5)
#define     USB_SETUP_REQUEST_SET_CONFIGURATION             (9)
#define     USB_SETUP_REQUEST_SET_DESCRIPTOR                (7)
#define     USB_SETUP_REQUEST_SET_FEATURE                   (3)
#define     USB_SETUP_REQUEST_SET_INTERFACE                 (11)
#define     USB_SETUP_REQUEST_SYNC_FRAME                    (12)

#define     USB_SETUP_DESCRIPTOR_DEVICE                     (1)
#define     USB_SETUP_DESCRIPTOR_CONFIG                     (2)
#define     USB_SETUP_DESCRIPTOR_STRING                     (3)
#define     USB_SETUP_DESCRIPTOR_INTERFACE                  (4)
#define     USB_SETUP_DESCRIPTOR_ENDPOINT                   (5)

struct __attribute__((packed)) usbdev_setup_packet {
    uint8_t     recipient:5;
    uint8_t     type:2;
    uint8_t     direction:1;
    uint8_t     request;
    uint16_t    value;
    uint16_t    index;
    uint16_t    length;
};

void init_usbdev(void) __attribute__((section(".kernel")));
int usbdev_tx(uint32_t *packet, int size) __attribute__((section(".kernel")));
void usbdev_rx(uint32_t *buf, int bytes) __attribute__((section(".kernel")));
void usbdev_handler(void) __attribute__((section(".kernel")));

#endif
