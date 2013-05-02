#include <stdint.h>
#include <stddef.h>
#include "usbdev_internals.h"
#include "usbdev_desc.h"

uint8_t usb_ready = 0;

uint8_t ep_ctl_rx_buf[4*USB_RX_FIFO_SIZE];

uint8_t *ep_tx_buf[4];

struct endpoint ep_ctl = {
    .num = 0,
    .dir = USB_DIR_IN,
    .mpsize = 64,
    .rx = {
        .buf = ep_ctl_rx_buf,
        .len = 4*USB_RX_FIFO_SIZE,
        .start = 0,
        .end = 0
    },
    .tx = {
        .buf = NULL,
        .len = 0,
        .start = 0,
        .end = 0
    },
    .request_disable = 0
};

struct endpoint ep_acm = {
    .num = USB_CDC_ACM_ENDPOINT,
    .dir = USB_DIR_OUT,
    .mpsize = USB_CDC_ACM_MPSIZE,
    .rx = {
        .buf = NULL,
        .len = 0,
        .start = 0,
        .end = 0
    },
    .tx = {
        .buf = NULL,
        .len = 0,
        .start = 0,
        .end = 0
    },
    .request_disable = 0
};

uint8_t ep_rx_buf[4*USB_RX_FIFO_SIZE];

struct endpoint ep_rx = {
    .num = USB_CDC_RX_ENDPOINT,
    .dir = USB_DIR_IN,
    .mpsize = USB_CDC_RX_MPSIZE,
    .rx = {
        .buf = ep_rx_buf,
        .len = 4*USB_RX_FIFO_SIZE,
        .start = 0,
        .end = 0
    },
    .tx = {
        .buf = NULL,
        .len = 0,
        .start = 0,
        .end = 0
    },
    .request_disable = 0
};

struct endpoint ep_tx = {
    .num = USB_CDC_TX_ENDPOINT,
    .dir = USB_DIR_OUT,
    .mpsize = USB_CDC_TX_MPSIZE,
    .rx = {
        .buf = NULL,
        .len = 0,
        .start = 0,
        .end = 0
    },
    .tx = {
        .buf = NULL,
        .len = 0,
        .start = 0,
        .end = 0
    },
    .request_disable = 0
};

struct endpoint *endpoints[4] = {NULL};

const struct usb_device_descriptor usb_device_descriptor = {
    .bLength =             sizeof(struct usb_device_descriptor),
    .bDescriptorType =     USB_SETUP_DESCRIPTOR_DEVICE,
    .bcdUSB =              USB_VERSION_1_1,
    .bDeviceClass =        USB_CLASS_CDC,
    .bDeviceSubClass =     0,
    .bDeviceProtocol =     0,
    .bMaxPacketSize0 =     64,
    .idVendor =            0x0483,     /* https://github.com/ericherman/stm32f4-discovery-example/blob/master/rotate-chars/rotate-chars-usb-descriptors.h */
    .idProduct =           0x5740,
    .bcdDevice =           1,
    .iManufacturer =       0,
    .iProduct =            0,
    .iSerialNumber =       0,
    .bNumConfigurations =  1
};

const struct usbdev_configuration1 usbdev_configuration1 = {
    {   /* Configuration Descriptor */
        /* WARNING DEFINED BELOW AS WELL*/
        .bLength =                       sizeof(struct usb_configuration_descriptor),
        .bDescriptorType =               USB_SETUP_DESCRIPTOR_CONFIG,
        .wTotalLength =                  sizeof(struct usbdev_configuration1),
        .bNumInterfaces =                2,
        .bConfigurationValue =           1,
        .iConfiguration =                0,
        .bmAttributes_reserved_0 =       0,
        .bmAttributes_remote_wakeup =    1,
        .bmAttributes_self_power =       1,
        .bmAttributes_reserved_1 =       1,
        .bMaxPower =                     0
    },
    {   /* Interface 0 Descriptor */
        .bLength =              sizeof(struct usb_interface_descriptor),
        .bDescriptorType =      USB_SETUP_DESCRIPTOR_INTERFACE,
        .bInterfaceNumber =     0,
        .bAlternateSetting =    0,
        .bNumEndpoints =        1,
        .bInterfaceClass =      USB_CLASS_CDC,
        .bInterfaceSubClass =   USB_CDC_SUBCLASS_ACM,
        .bInterfaceProtocol =   USB_CDC_PROTOCOL_AT,
        .iInterface =           0
    },
    {   /* CDC Header Functional Descriptor */
        .bFunctionLength =      sizeof(struct usb_cdc_header_functional_descriptor),
        .bDescriptorType =      USB_CDC_DESCRIPTOR_CS_INTERFACE,
        .bDescriptorSubtype =   USB_CDC_DESCRIPTOR_SUBTYPE_HEADER,
        .bcdCDC =               USB_CDC_VERSION_1_1
    },
    {   /* CDC Call Management Functional Descriptor */
        .bFunctionLength =                              sizeof(struct usb_cdc_call_management_functional_descriptor),
        .bDescriptorType =                              USB_CDC_DESCRIPTOR_CS_INTERFACE,
        .bDcescriptorSubtype =                          USB_CDC_DESCRIPTOR_SUBTYPE_CALL_MANAGEMENT,
        .bmCapabilities_handle_call_management =        1,
        .bmCapabilities_call_management_on_data_class = 0,
        .bmCapabilities_reserved_0 =                    0,
        .bDataInterface =                               1
    },
    {   /* CDC ACM Functional Descriptor */
        .bFunctionLength =                          sizeof(struct usb_cdc_acm_functional_descriptor),
        .bDescriptorType =                          USB_CDC_DESCRIPTOR_CS_INTERFACE,
        .bDcescriptorSubtype =                      USB_CDC_DESCRIPTOR_SUBTYPE_ACM,
        .bmCapabilities_support_comm_feature =      0,
        .bmCapabilities_support_line_coding =       1,
        .bmCapabilities_support_send_break =        1,
        .bmCapabilities_support_network_connect =   0,
        .bmCapabilities_reserved_0 =                0
    },
    {   /* CDC Union Functional Descriptor */
        .bFunctionLength =      sizeof(struct usb_cdc_acm_union_functional_descriptor),
        .bDescriptorType =      USB_CDC_DESCRIPTOR_CS_INTERFACE,
        .bDcescriptorSubtype =  USB_CDC_DESCRIPTOR_SUBTYPE_UNION,
        .bMasterInterface =     0,
        .bSlaveInterface0 =     1
    },
    {   /* Endpoint ACM Descriptor */
        .bLength =                      sizeof(struct usb_endpoint_descriptor),
        .bDescriptorType =              USB_SETUP_DESCRIPTOR_ENDPOINT,
        .bEndpointAddress_number =      USB_CDC_ACM_ENDPOINT,
        .bEndpointAddress_reserved_0 =  0,
        .bEndpointAddress_direction =   USB_SETUP_ENDPOINT_DIR_IN,
        .bmAttributes_transfer_type =   USB_SETUP_ENDPOINT_TRANSFER_INTERRUPT,
        .bmAttributes_sync_type =       USB_SETUP_ENDPOINT_SYNC_NONE,
        .bmAttributes_usage_type =      0,
        .bmAttributes_reserved_0 =      0,
        .wMaxPacketSize =               USB_CDC_ACM_MPSIZE,
        .bInterval =                    64
    },
    {   /* Interface 1 Descriptor */
        .bLength =              sizeof(struct usb_interface_descriptor),
        .bDescriptorType =      USB_SETUP_DESCRIPTOR_INTERFACE,
        .bInterfaceNumber =     1,
        .bAlternateSetting =    0,
        .bNumEndpoints =        2,
        .bInterfaceClass =      USB_CLASS_CDC_DATA,
        .bInterfaceSubClass =   0,
        .bInterfaceProtocol =   0,
        .iInterface =           0
    },
    {   /* Endpoint RX Descriptor */
        .bLength =                      sizeof(struct usb_endpoint_descriptor),
        .bDescriptorType =              USB_SETUP_DESCRIPTOR_ENDPOINT,
        .bEndpointAddress_number =      USB_CDC_RX_ENDPOINT,
        .bEndpointAddress_reserved_0 =  0,
        .bEndpointAddress_direction =   USB_SETUP_ENDPOINT_DIR_OUT,
        .bmAttributes_transfer_type =   USB_SETUP_ENDPOINT_TRANSFER_BULK,
        .bmAttributes_sync_type =       USB_SETUP_ENDPOINT_SYNC_NONE,
        .bmAttributes_usage_type =      0,
        .bmAttributes_reserved_0 =      0,
        .wMaxPacketSize =               USB_CDC_RX_MPSIZE,
        .bInterval =                    0
    },
    {   /* Endpoint TX Descriptor */
        .bLength =                      sizeof(struct usb_endpoint_descriptor),
        .bDescriptorType =              USB_SETUP_DESCRIPTOR_ENDPOINT,
        .bEndpointAddress_number =      USB_CDC_TX_ENDPOINT,
        .bEndpointAddress_reserved_0 =  0,
        .bEndpointAddress_direction =   USB_SETUP_ENDPOINT_DIR_IN,
        .bmAttributes_transfer_type =   USB_SETUP_ENDPOINT_TRANSFER_BULK,
        .bmAttributes_sync_type =       USB_SETUP_ENDPOINT_SYNC_NONE,
        .bmAttributes_usage_type =      0,
        .bmAttributes_reserved_0 =      0,
        .wMaxPacketSize =               USB_CDC_TX_MPSIZE,
        .bInterval =                    0
    }
};

const struct usb_configuration_descriptor usbdev_configuration1_descriptor = {
        /* WARNING DEFINED ABOVE AS WELL*/
        .bLength =                       sizeof(struct usb_configuration_descriptor),
        .bDescriptorType =               USB_SETUP_DESCRIPTOR_CONFIG,
        .wTotalLength =                  sizeof(struct usbdev_configuration1),
        .bNumInterfaces =                2,
        .bConfigurationValue =           1,
        .iConfiguration =                0,
        .bmAttributes_reserved_0 =       0,
        .bmAttributes_remote_wakeup =    1,
        .bmAttributes_self_power =       1,
        .bmAttributes_reserved_1 =       1,
        .bMaxPower =                     0
};

