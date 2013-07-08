#include <stdint.h>
#include <arch/chip/registers.h>
#include <kernel/fault.h>

#include "usbdev_internals.h"
#include "usbdev_desc.h"
#include "usbdev_class.h"
#include <dev/hw/usbdev.h>

static void std_setup_packet(struct usbdev_setup_packet *setup);
static void cdc_setup_packet(struct usbdev_setup_packet *setup);
static void cdc_set_configuration(uint16_t configuration);

void usbdev_setup(struct ring_buffer *packet, uint32_t len) {
    uint8_t buf[8];
    for (int i = 0; i < 8; i++) {
        if (ring_buf_empty(packet)) {
            break;
        }
        buf[i] = packet->buf[packet->start];
        packet->start = (packet->start + 1) % packet->len;
    }

    /* Clear ring buffer */
    packet->start = 0;
    packet->end = 0;

    struct usbdev_setup_packet *setup = (struct usbdev_setup_packet *) buf;

    switch (setup->type) {
    case USB_SETUP_REQUEST_TYPE_TYPE_STD:
        std_setup_packet(setup);
        break;
    case USB_SETUP_REQUEST_TYPE_TYPE_CLASS:
        cdc_setup_packet(setup);
        break;
    default:
        DEBUG_PRINT("Unhandled SETUP packet, type %d. ", setup->type);
    }
}

static void std_setup_packet(struct usbdev_setup_packet *setup) {
    switch (setup->request) {
    case USB_SETUP_REQUEST_GET_DESCRIPTOR:
        DEBUG_PRINT("GET_DESCRIPTOR ");
        switch (setup->value >> 8) {
        case USB_SETUP_DESCRIPTOR_DEVICE:
            DEBUG_PRINT("DEVICE ");
            usbdev_write(endpoints[0], (uint8_t *) &usb_device_descriptor, sizeof(struct usb_device_descriptor));
            break;
        case USB_SETUP_DESCRIPTOR_CONFIG:
            DEBUG_PRINT("CONFIGURATION ");
            if (setup->length <= sizeof(usbdev_configuration1_descriptor)) {
                usbdev_write(endpoints[0], (uint8_t *) &usbdev_configuration1_descriptor, sizeof(usbdev_configuration1_descriptor));
            }
            else {
                usbdev_write(endpoints[0], (uint8_t *) &usbdev_configuration1, sizeof(usbdev_configuration1));
            }
            break;
        default:
            DEBUG_PRINT("OTHER DESCRIPTOR %d ", setup->value >> 8);
        }
        break;
    case USB_SETUP_REQUEST_SET_ADDRESS:
        DEBUG_PRINT("SET_ADDRESS %d ", setup->value);
        *USB_FS_DCFG |= USB_FS_DCFG_DAD(setup->value);
        usbdev_status_in_packet();
        break;
    case USB_SETUP_REQUEST_SET_CONFIGURATION:
        DEBUG_PRINT("SET_CONFIGURATION %d ", setup->value);
        cdc_set_configuration(setup->value);
        usbdev_status_in_packet();
        break;
    case USB_SETUP_REQUEST_GET_STATUS:
        DEBUG_PRINT("GET_STATUS ");
        if (setup->recipient == USB_SETUP_REQUEST_TYPE_RECIPIENT_DEVICE) {
            DEBUG_PRINT("DEVICE ");
            uint8_t buf = 0x11; /* Self powered and remote wakeup */
            usbdev_write(endpoints[0], &buf, sizeof(buf));
        }
        else {
            DEBUG_PRINT("OTHER ");
        }
        break;
    default:
        DEBUG_PRINT("STD: OTHER_REQUEST %d ", setup->request);
    }
}

static void cdc_setup_packet(struct usbdev_setup_packet *setup) {
    switch (setup->request) {
    case USB_SETUP_REQUEST_CDC_SET_CONTROL_LINE_STATE:
        DEBUG_PRINT("CDC: SET_CONTROL_LINE_STATE Warning: Not handled ");
        usbdev_status_in_packet();
        break;
    case USB_SETUP_REQUEST_CDC_SET_LINE_CODING:
        DEBUG_PRINT("CDC: SET_LINE_CODING Warning: Not handled ");
        usbdev_status_in_packet();
        break;
    default:
        DEBUG_PRINT("CDC: OTHER_REQUEST %d ", setup->request);
    }
}

static void cdc_set_configuration(uint16_t configuration) {
    if (configuration != 1) {
        DEBUG_PRINT("Warning: Cannot set configuration %u. ", configuration);
    }

    DEBUG_PRINT("Setting configuration %u. ", configuration);

    /* ACM Endpoint */
    *USB_FS_DIEPCTL(USB_CDC_ACM_ENDPOINT) |= USB_FS_DIEPCTLx_MPSIZE(USB_CDC_ACM_MPSIZE) | USB_FS_DIEPCTLx_EPTYP_INT | USB_FS_DIEPCTLx_TXFNUM(USB_CDC_ACM_ENDPOINT) | USB_FS_DIEPCTLx_USBAEP;

    /* RX Endpoint */
    *USB_FS_DOEPCTL(USB_CDC_RX_ENDPOINT) |= USB_FS_DOEPCTLx_MPSIZE(USB_CDC_RX_MPSIZE) | USB_FS_DOEPCTLx_EPTYP_BLK | USB_FS_DOEPCTLx_SD0PID | USB_FS_DOEPCTLx_EPENA | USB_FS_DOEPCTLx_USBAEP;

    /* TX Endpoint */
    *USB_FS_DIEPCTL(USB_CDC_TX_ENDPOINT) |= USB_FS_DIEPCTLx_MPSIZE(USB_CDC_TX_MPSIZE) | USB_FS_DIEPCTLx_EPTYP_BLK | USB_FS_DIEPCTLx_SD0PID | USB_FS_DIEPCTLx_TXFNUM(USB_CDC_TX_ENDPOINT) | USB_FS_DIEPCTLx_USBAEP;

    /* Flush TX FIFOs */
    *USB_FS_GRSTCTL |= USB_FS_GRSTCTL_TXFNUM(USB_CDC_TX_ENDPOINT) | USB_FS_GRSTCTL_TXFNUM(USB_CDC_ACM_ENDPOINT) | USB_FS_GRSTCTL_TXFFLSH;
    while (*USB_FS_GRSTCTL & USB_FS_GRSTCTL_TXFFLSH);

    /* Unmask interrupts */
    *USB_FS_DAINTMSK |= USB_FS_DAINT_IEPM(USB_CDC_ACM_ENDPOINT) | USB_FS_DAINT_IEPM(USB_CDC_TX_ENDPOINT) | USB_FS_DAINT_OEPM(USB_CDC_RX_ENDPOINT);
    *USB_FS_DIEPMSK |= USB_FS_DIEPMSK_XFRCM;

    endpoints[USB_CDC_ACM_ENDPOINT] = &ep_acm;
    endpoints[USB_CDC_RX_ENDPOINT] = &ep_rx;
    endpoints[USB_CDC_TX_ENDPOINT] = &ep_tx;

    if (endpoints[1]) {
        endpoints[1]->tx.buf = ep_tx_buf[1];
        endpoints[1]->tx.len = 4*USB_TX1_FIFO_SIZE;
        endpoints[1]->tx.start = 0;
        endpoints[1]->tx.end = 0;
    }
    if (endpoints[2]) {
        endpoints[2]->tx.buf = ep_tx_buf[2];
        endpoints[2]->tx.len = 4*USB_TX2_FIFO_SIZE;
        endpoints[2]->tx.start = 0;
        endpoints[2]->tx.end = 0;
    }
    if (endpoints[3]) {
        endpoints[3]->tx.buf = ep_tx_buf[3];
        endpoints[3]->tx.len = 4*USB_TX3_FIFO_SIZE;
        endpoints[3]->tx.start = 0;
        endpoints[3]->tx.end = 0;
    }

    usb_ready = 1;

    usbdev_enable_receive(&ep_rx);
}
