#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <kernel/fault.h>
#include <kernel/sched.h>
#include <dev/registers.h>

#include "usbdev_internals.h"
#include "usbdev_desc.h"
#include <dev/hw/usbdev.h>

/* packet points to first word in packet.  size is packet size in bytes */
void usbdev_write(struct endpoint *ep, uint32_t *packet, int size) {
    if (ep == NULL) {
        DEBUG_PRINT("Warning: Invalid endpoint in usbdev_write. ");
        return;
    }
    if (ep->tx.buf == NULL) {
        DEBUG_PRINT("Warning: Endpoint has no tx buffer in usbdev_write. ");
        return;
    }

    /* Wait until current buffer is empty */
    while (!ring_buf_empty(&ep->tx)) {
        if (task_switching && !IPSR()) {
            SVC(SVC_YIELD);
        }
    }

    uint8_t packets = size % ep->mpsize ? size/ep->mpsize + 1 : size/ep->mpsize;
    if (!packets) {
        packets = 1;
    }

    int filled_buffer = 0;
    int written = 0;

    /* Copy to ring buffer */
    while (size > 0 && !ring_buf_full(&ep->tx)) {
        ep->tx.buf[ep->tx.end] = *packet++;
        size -= 4;
        written += 4;

        ep->tx.end = (ep->tx.end + 1) % ep->tx.len;
    }

    /* If we write the entire buffer, but it didn't have a multiple of 4 bytes
     * size will be negative.  This means that written is actually too large,
     * we need to scale it back to the appropriate value */
    if (size < 0) {
        written += size;
    }

    if (ring_buf_full(&ep->tx)) {
        filled_buffer = 1;
    }

    /* Setup endpoint for transmit */
    if (ep->num == 0) {
        *USB_FS_DIEPTSIZ0 = USB_FS_DIEPTSIZ0_PKTCNT(packets) | USB_FS_DIEPTSIZ0_XFRSIZ(written);
        *USB_FS_DIEPCTL0 |= USB_FS_DIEPCTL0_CNAK | USB_FS_DIEPCTL0_EPENA;
    }
    else {
        *USB_FS_DIEPTSIZ(ep->num) = USB_FS_DIEPTSIZx_PKTCNT(packets) | USB_FS_DIEPTSIZx_XFRSIZ(written);
        *USB_FS_DIEPCTL(ep->num) |= USB_FS_DIEPCTLx_CNAK | USB_FS_DIEPCTLx_EPENA;
    }

    /* Enable TX FIFO empty interrupt */
    *USB_FS_DIEPEMPMSK |= (1 << ep->num);

    /* Filled buffer, call recursively until packet finishes */
    if (filled_buffer) {
        usbdev_write(ep, packet, size);
    }
}

void usbdev_fifo_read(volatile struct ring_buffer *ring, int words) {
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
        while (words > 0) {
            ring->buf[ring->end] = *USB_FS_DFIFO_EP(0);
            words--;

            if (ring_buf_full(ring)) {
                DEBUG_PRINT("Warning: USB: Buffer full.\r\n");
                ring->start = (ring->start + 1) % ring->len;
            }
            ring->end = (ring->end + 1) % ring->len;
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

    uint32_t words = (size + 3) / 4;

    usbdev_fifo_read(&ep->rx, words);
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
        DEBUG_PRINT("0x%x ", ep->tx.buf[ep->tx.start]);
        *USB_FS_DFIFO_EP(ep->num) = ep->tx.buf[ep->tx.start];
        ep->tx.start = (ep->tx.start + 1) % ep->tx.len;
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
