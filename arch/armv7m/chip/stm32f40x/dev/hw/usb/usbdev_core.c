#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <kernel/fault.h>
#include <kernel/sched.h>
#include <arch/system.h>
#include <arch/chip/registers.h>

#include "usbdev_internals.h"
#include "usbdev_desc.h"
#include <dev/hw/usbdev.h>

union uint8_uint32 {
    uint8_t     uint8[4];
    uint32_t    uint32;
};

/* packet points to first byte in packet.  size is packet size in bytes */
/* Returns bytes written, negative on error */
int usbdev_write(struct endpoint *ep, uint8_t *packet, int size) {
    if (ep == NULL) {
        DEBUG_PRINT("Warning: Invalid endpoint in usbdev_write. ");
        return -1;
    }
    if (ep->num != 0 && !usb_ready) {
        return -1;
    }
    if (ep->tx.buf == NULL) {
        DEBUG_PRINT("Warning: Endpoint has no tx buffer in usbdev_write. ");
        return -1;
    }

    /* Wait until current buffer is empty */
    while (!ring_buf_empty(&ep->tx)) {
        yield_if_possible();
    }

    int filled_buffer = 0;
    int written = 0;

    /* Copy to ring buffer */
    while (size > 0 && !ring_buf_full(&ep->tx)) {
        ep->tx.buf[ep->tx.end] = *packet++;
        size--;
        written++;

        ep->tx.end = (ep->tx.end + 1) % ep->tx.len;
    }

    if (ring_buf_full(&ep->tx)) {
        filled_buffer = 1;
    }

    uint8_t packets = written % ep->mpsize ? written/ep->mpsize + 1 : written/ep->mpsize;
    if (!packets) {
        packets = 1;
    }

    int count = 500;
    /* Setup endpoint for transmit */
    if (ep->num == 0) {
        /* Wait for ep to disable */
        while (*USB_FS_DIEPCTL0 & USB_FS_DIEPCTL0_EPENA && count > 0) {
            count--;
        }

        /* Abort timed out transfer */
        if (count <= 0) {
            ep->request_disable = 1;

            *USB_FS_DIEPCTL0 |= USB_FS_DIEPCTL0_SNAK;
            *USB_FS_DIEPMSK |= USB_FS_DIEPMSK_INEPNEM | USB_FS_DIEPMSK_EPDM;

            /* Wait for endpoint to disable */
            while(ep->request_disable);

            *USB_FS_DIEPMSK &= ~(USB_FS_DIEPMSK_INEPNEM | USB_FS_DIEPMSK_EPDM);
        }

        *USB_FS_DIEPTSIZ0 = USB_FS_DIEPTSIZ0_PKTCNT(packets) | USB_FS_DIEPTSIZ0_XFRSIZ(written);
        *USB_FS_DIEPCTL0 |= USB_FS_DIEPCTL0_CNAK | USB_FS_DIEPCTL0_EPENA;
    }
    else {
        /* Wait for ep to disable */
        while (*USB_FS_DIEPCTL(ep->num) & USB_FS_DIEPCTLx_EPENA && count > 0) {
            count--;
        }

        /* Abort timed out transfer */
        if (count <= 0) {
            ep->request_disable = 1;

            *USB_FS_DIEPCTL(ep->num) |= USB_FS_DIEPCTLx_SNAK;
            *USB_FS_DIEPMSK |= USB_FS_DIEPMSK_INEPNEM | USB_FS_DIEPMSK_EPDM;

            /* Wait for endpoint to disable */
            while(ep->request_disable);

            *USB_FS_DIEPMSK &= ~(USB_FS_DIEPMSK_INEPNEM | USB_FS_DIEPMSK_EPDM);
        }

        *USB_FS_DIEPTSIZ(ep->num) = USB_FS_DIEPTSIZx_PKTCNT(packets) | USB_FS_DIEPTSIZx_XFRSIZ(written);
        *USB_FS_DIEPCTL(ep->num) |= USB_FS_DIEPCTLx_CNAK | USB_FS_DIEPCTLx_EPENA;
    }

    /* Enable TX FIFO empty interrupt */
    *USB_FS_DIEPEMPMSK |= (1 << ep->num);

    /* Filled buffer, call recursively until packet finishes */
    if (filled_buffer && size) {
        int ret = usbdev_write(ep, packet, size);
        if (ret >= 0) {
            written += ret;
        }
        else {
            written = ret;
        }
    }

    return written;
}

void usbdev_fifo_read(volatile struct ring_buffer *ring, int size) {
    int words = (size+3)/4;

    /* Allow us to read into NULL */
    if (ring == NULL) {
        uint32_t null;
        while (words > 0) {
            null = *USB_FS_DFIFO_EP(0);
            words--;
        }
        /* Tricks GCC into thinking null is used */
        if (null) {
            return;
        }
    }
    else {
        while (words > 0 && size > 0) {
            union uint8_uint32 data;
            data.uint32 = *USB_FS_DFIFO_EP(0);
            words--;

            for (int i = 0; i < 4; i++) {
                if (size <= 0) {
                    data.uint8[i] = 0;
                    continue;
                }

                ring->buf[ring->end] = data.uint8[i];
                size--;

                if (ring_buf_full(ring)) {
                    DEBUG_PRINT("Warning: USB: Buffer full.\r\n");
                    ring->start = (ring->start + 1) % ring->len;
                }
                ring->end = (ring->end + 1) % ring->len;
            }
        }
    }
}

void usbdev_data_out(uint32_t status) {
    uint32_t size = USB_FS_GRXSTS_BCNT(status);
    struct endpoint *ep = endpoints[USB_FS_GRXSTS_EPNUM(status)];

    if (ep == NULL) {
        DEBUG_PRINT("Warning: Invalid endpoint in usbdev_data_out. ");
        return;
    }

    usbdev_fifo_read(&ep->rx, size);
}

void usbdev_data_in(struct endpoint *ep) {
    if (ep == NULL) {
        DEBUG_PRINT("Warning: Invalid endpoint in usbdev_data_in. ");
        return;
    }

    if (!ep->tx.buf) {
        return;
    }

    DEBUG_PRINT("Writing FIFO %d: ", ep->num);

    /* Write until buffer empty */
    int written = 0;
    int space = *USB_FS_DTXFSTS(ep->num);
    while (written < space && !ring_buf_empty(&ep->tx)) {
        union uint8_uint32 data;
        data.uint32 = 0;

        for (int i = 0; i < 4; i++) {
            if (!ring_buf_empty(&ep->tx)) {
                data.uint8[i] = ep->tx.buf[ep->tx.start];
                ep->tx.start = (ep->tx.start + 1) % ep->tx.len;
            }
            else {
                data.uint8[i] = 0;
            }
        }

        DEBUG_PRINT("0x%x ", data.uint32);

        *USB_FS_DFIFO_EP(ep->num) = data.uint32;
        written++;
    }

    /* Only disable interrupt once all data has been written */
    if (ring_buf_empty(&ep->tx)) {
        *USB_FS_DIEPEMPMSK &= ~(1 << ep->num);
    }
}

void usbdev_status_in_packet(void) {
    *USB_FS_DIEPTSIZ0 = USB_FS_DIEPTSIZ0_PKTCNT(1) | USB_FS_DIEPTSIZ0_XFRSIZ(0);
    *USB_FS_DIEPCTL0 |= USB_FS_DIEPCTL0_CNAK | USB_FS_DIEPCTL0_EPENA;

    *USB_FS_DOEPTSIZ0 = USB_FS_DOEPTSIZ0_STUPCNT(3) | USB_FS_DOEPTSIZ0_PKTCNT(1) | USB_FS_DOEPTSIZ0_XFRSIZ(8*3);
}

void usbdev_enable_receive(struct endpoint *ep) {
    if (ep == NULL) {
        DEBUG_PRINT("Warning: Invalid endpoint in usbdev_enable_receive ");
        return;
    }

    if (ep->num == 0) {
        *USB_FS_DOEPTSIZ0 = USB_FS_DOEPTSIZ0_XFRSIZ(ep->mpsize) | USB_FS_DOEPTSIZ0_PKTCNT(1) | USB_FS_DOEPTSIZ0_STUPCNT(3);
        *USB_FS_DOEPCTL0 |= USB_FS_DOEPCTL0_CNAK | USB_FS_DOEPCTL0_EPENA;
    }
    else {
        *USB_FS_DOEPTSIZ(ep->num) = USB_FS_DOEPTSIZx_XFRSIZ(ep->mpsize) | USB_FS_DOEPTSIZx_PKTCNT(1);
        *USB_FS_DOEPCTL(ep->num) |= USB_FS_DOEPCTLx_CNAK | USB_FS_DOEPCTLx_EPENA;
    }
}
