#ifndef USBDEV_INTERNALS_H_INCLUDED
#define USBDEV_INTERNALS_H_INCLUDED

#define     USB_VERSION_1_1                                 (0x110)
#define     USB_CLASS_CDC                                   (0x02)
#define     USB_CLASS_CDC_DATA                              (0x0A)

/* CDC See USB CDC spec */
#define     USB_CDC_VERSION_1_1                             (0x110)

#define     USB_CDC_SUBCLASS_ACM                            (0x02)
#define     USB_CDC_PROTOCOL_AT                             (0x01)

#define     USB_CDC_DESCRIPTOR_CS_INTERFACE                 (0x24)
#define     USB_CDC_DESCRIPTOR_CS_ENDPOINT                  (0x26)

#define     USB_CDC_DESCRIPTOR_SUBTYPE_HEADER               (0x00)
#define     USB_CDC_DESCRIPTOR_SUBTYPE_CALL_MANAGEMENT      (0x01)
#define     USB_CDC_DESCRIPTOR_SUBTYPE_ACM                  (0x02)
#define     USB_CDC_DESCRIPTOR_SUBTYPE_UNION                (0x06)

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
#define     USB_SETUP_REQUEST_TYPE_DIR_D2H                  (1)

#define     USB_DIR_OUT                                     (0)
#define     USB_DIR_IN                                      (1)

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

#define     USB_SETUP_ENDPOINT_DIR_OUT                      (0)
#define     USB_SETUP_ENDPOINT_DIR_IN                       (1)

#define     USB_SETUP_ENDPOINT_TRANSFER_CONTROL             (0)
#define     USB_SETUP_ENDPOINT_TRANSFER_ISOSYNC             (1)
#define     USB_SETUP_ENDPOINT_TRANSFER_BULK                (2)
#define     USB_SETUP_ENDPOINT_TRANSFER_INTERRUPT           (3)

#define     USB_SETUP_ENDPOINT_SYNC_NONE                    (0)
#define     USB_SETUP_ENDPOINT_SYNC_ASYNC                   (1)
#define     USB_SETUP_ENDPOINT_SYNC_ADAPT                   (2)
#define     USB_SETUP_ENDPOINT_SYNC_SYNC                    (3)

/* CDC Requests */
#define     USB_SETUP_REQUEST_CDC_SET_CONTROL_LINE_STATE            (0x22)
#define     USB_SETUP_REQUEST_CDC_SET_CONTROL_LINE_STATE_DTE        (1 << 0)
#define     USB_SETUP_REQUEST_CDC_SET_CONTROL_LINE_STATE_CARRIER    (1 << 1)

#define     USB_SETUP_REQUEST_CDC_SET_LINE_CODING           (0x20)

struct __attribute__((packed)) usbdev_setup_packet {
    uint8_t     recipient:5;
    uint8_t     type:2;
    uint8_t     direction:1;
    uint8_t     request;
    uint16_t    value;
    uint16_t    index;
    uint16_t    length;
};

struct __attribute__((packed)) usb_device_descriptor {
    uint8_t     bLength;
    uint8_t     bDescriptorType;
    uint16_t    bcdUSB;
    uint8_t     bDeviceClass;
    uint8_t     bDeviceSubClass;
    uint8_t     bDeviceProtocol;
    uint8_t     bMaxPacketSize0;
    uint16_t    idVendor;
    uint16_t    idProduct;
    uint16_t    bcdDevice;
    uint8_t     iManufacturer;
    uint8_t     iProduct;
    uint8_t     iSerialNumber;
    uint8_t     bNumConfigurations;
};

struct __attribute__((packed)) usb_configuration_descriptor {
    uint8_t     bLength;
    uint8_t     bDescriptorType;
    uint16_t    wTotalLength;
    uint8_t     bNumInterfaces;
    uint8_t     bConfigurationValue;
    uint8_t     iConfiguration;
    uint8_t     bmAttributes_reserved_0 : 5;
    uint8_t     bmAttributes_remote_wakeup : 1;
    uint8_t     bmAttributes_self_power : 1;
    uint8_t     bmAttributes_reserved_1 : 1;
    uint8_t     bMaxPower;
};

struct __attribute__((packed)) usb_interface_descriptor {
    uint8_t     bLength;
    uint8_t     bDescriptorType;
    uint8_t     bInterfaceNumber;
    uint8_t     bAlternateSetting;
    uint8_t     bNumEndpoints;
    uint8_t     bInterfaceClass;
    uint8_t     bInterfaceSubClass;
    uint8_t     bInterfaceProtocol;
    uint8_t     iInterface;
};

struct __attribute__((packed)) usb_endpoint_descriptor {
    uint8_t     bLength;
    uint8_t     bDescriptorType;
    uint8_t     bEndpointAddress_number : 4;
    uint8_t     bEndpointAddress_reserved_0 : 3;
    uint8_t     bEndpointAddress_direction : 1;
    uint8_t     bmAttributes_transfer_type : 2;
    uint8_t     bmAttributes_sync_type : 2;
    uint8_t     bmAttributes_usage_type : 2;
    uint8_t     bmAttributes_reserved_0 : 2;
    uint16_t    wMaxPacketSize;
    uint8_t     bInterval;
};

/* See USB CDC Spec starting pg 34 */

struct __attribute__((packed)) usb_cdc_header_functional_descriptor {
    uint8_t     bFunctionLength;
    uint8_t     bDescriptorType;
    uint8_t     bDescriptorSubtype;
    uint16_t    bcdCDC;
};

struct __attribute__((packed)) usb_cdc_call_management_functional_descriptor {
    uint8_t     bFunctionLength;
    uint8_t     bDescriptorType;
    uint8_t     bDcescriptorSubtype;
    uint8_t     bmCapabilities_handle_call_management : 1;
    uint8_t     bmCapabilities_call_management_on_data_class : 1;
    uint8_t     bmCapabilities_reserved_0 : 6;
    uint8_t     bDataInterface;
};

struct __attribute__((packed)) usb_cdc_acm_functional_descriptor {
    uint8_t     bFunctionLength;
    uint8_t     bDescriptorType;
    uint8_t     bDcescriptorSubtype;
    uint8_t     bmCapabilities_support_comm_feature : 1;
    uint8_t     bmCapabilities_support_line_coding : 1;
    uint8_t     bmCapabilities_support_send_break : 1;
    uint8_t     bmCapabilities_support_network_connect : 1;
    uint8_t     bmCapabilities_reserved_0 : 4;
};

struct __attribute__((packed)) usb_cdc_acm_union_functional_descriptor {
    uint8_t     bFunctionLength;
    uint8_t     bDescriptorType;
    uint8_t     bDcescriptorSubtype;
    uint8_t     bMasterInterface;
    uint8_t     bSlaveInterface0;
};

/* Ring buffer semantics always leave one space empty.
 * If start == end, buffer is empty */
struct ring_buffer {
    volatile uint8_t    *buf;
    volatile int        len;
    volatile int        start;
    volatile int        end;
};

struct endpoint {
    uint8_t             num;
    uint8_t             dir;
    uint16_t            mpsize;
    volatile struct ring_buffer  rx;
    volatile struct ring_buffer  tx;
    volatile uint8_t    request_disable;
};

void usbdev_reset(void);
int usbdev_write(struct endpoint *ep, uint8_t *packet, int size);
void usbdev_fifo_read(volatile struct ring_buffer *ring, int size);
void usbdev_data_out(uint32_t status);
void usbdev_data_in(struct endpoint *ep);
void usbdev_status_in_packet(void);
void usbdev_enable_receive(struct endpoint *ep);

inline static int ring_buf_full(volatile struct ring_buffer *ring);
inline static int ring_buf_full(volatile struct ring_buffer *ring) {
    if (ring->start - ring->end == 1) {
        return 1;
    }
    if (ring->start == 0 && ring->end == (ring->len-1)) {
        return 1;
    }
    return 0;
}

inline static int ring_buf_empty(volatile struct ring_buffer *ring);
inline static int ring_buf_empty(volatile struct ring_buffer *ring) {
    if (ring->start == ring->end) {
        return 1;
    }
    return 0;
}

#endif
