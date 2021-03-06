/*
 * Copyright (C) 2013 F4OS Authors
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is furnished to do
 * so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef DEV_HW_USBDEV_DESC_H_INCLUDED
#define DEV_HW_USBDEV_DESC_H_INCLUDED

#include "usbdev_internals.h"

#define USB_RX_FIFO_SIZE    128
#define USB_TX0_FIFO_SIZE   32
#define USB_TX1_FIFO_SIZE   16
#define USB_TX2_FIFO_SIZE   128
#define USB_TX3_FIFO_SIZE   16

#define USB_CDC_ACM_ENDPOINT    (1)
#define USB_CDC_ACM_MPSIZE      (64)

#define USB_CDC_TX_ENDPOINT     (2)
#define USB_CDC_TX_MPSIZE       (64)

#define USB_CDC_RX_ENDPOINT     (3)
#define USB_CDC_RX_MPSIZE       (64)

struct __attribute__((packed)) usbdev_configuration1 {
    struct usb_configuration_descriptor                     config;
    struct usb_interface_descriptor                         interface0;
    struct usb_cdc_header_functional_descriptor             interface0_header;
    struct usb_cdc_call_management_functional_descriptor    interface0_call_man;
    struct usb_cdc_acm_functional_descriptor                interface0_acm;
    struct usb_cdc_acm_union_functional_descriptor          interface0_union;
    struct usb_endpoint_descriptor                          interface0_endpoint;
    struct usb_interface_descriptor                         interface1;
    struct usb_endpoint_descriptor                          interface1_endpoint1;
    struct usb_endpoint_descriptor                          interface1_endpoint2;
};


extern uint8_t usb_ready;
extern uint8_t *ep_tx_buf[];
extern struct endpoint ep_ctl;
extern struct endpoint ep_acm;
extern struct endpoint ep_rx;
extern struct endpoint ep_tx;
extern struct endpoint *endpoints[];

extern const struct usb_device_descriptor usb_device_descriptor;
extern const struct usbdev_configuration1 usbdev_configuration1;
extern const struct usb_configuration_descriptor usbdev_configuration1_descriptor;

#endif
