#include <stdint.h>
#include <stddef.h>
#include <kernel/fault.h>
#include <kernel/sched.h>
#include <dev/registers.h>

#include "usbdev_internals.h"
#include "usbdev_desc.h"
#include <dev/hw/usbdev.h>

/* packet points to first word in packet.  size is packet size in bytes */
void usbdev_write(struct endpoint *ep, uint32_t *packet, int size) {
    if (ep == NULL) {
        printk("Warning: Invalid endpoint in usbdev_write. ");
        return;
    }

    /* Wait until current buffer has been written */
    if (task_switching && !IPSR()) {
        while (ep->buf) {
            SVC(SVC_YIELD);
        }
    }
    else {
        while(ep->buf);
    }

    uint8_t packets = size % ep->mpsize ? size/ep->mpsize + 1 : size/ep->mpsize;
    if (!packets) {
        packets = 1;
    }

    ep->buf = packet;
    ep->buf_len = size;

    if (ep->num == 0) {
        *USB_FS_DIEPTSIZ0 = USB_FS_DIEPTSIZ0_PKTCNT(packets) | USB_FS_DIEPTSIZ0_XFRSIZ(size);
        *USB_FS_DIEPCTL0 |= USB_FS_DIEPCTL0_CNAK | USB_FS_DIEPCTL0_EPENA;
    }
    else {
        *USB_FS_DIEPTSIZ(ep->num) = USB_FS_DIEPTSIZx_PKTCNT(packets) | USB_FS_DIEPTSIZx_XFRSIZ(size);
        *USB_FS_DIEPCTL(ep->num) |= USB_FS_DIEPCTLx_CNAK | USB_FS_DIEPCTLx_EPENA;
    }

    /* Enable TX FIFO empty interrupt */
    *USB_FS_DIEPEMPMSK |= (1 << ep->num);
}

void usbdev_fifo_read(uint32_t *buf, int words) {
    uint32_t keep = (uint32_t) buf;
    uint32_t null;

    /* Allow us to read into NULL */
    if (!keep) {
        buf = &null;
    }

    while (words > 0) {
        *buf = *USB_FS_DFIFO_EP(0);
        if (keep) {
            buf++;
        }
        words--;
    }
}

void usbdev_data_out(uint32_t status) {
    uint32_t size = USB_FS_GRXSTS_BCNT(status);
    struct endpoint *ep = endpoints[USB_FS_GRXSTS_EPNUM(status)];

    if (ep == NULL) {
        printk("Warning: Invalid endpoint in usbdev_data_out. ");
        return;
    }

    uint32_t words = (size + 3) / 4;
    uint32_t extra = 0;

    if (size > ep->buf_len) {
        printk("Warning: Not enough room in buffer, only reading some data. ");
        words = ep->buf_len;
        extra = (ep->buf_len - size + 3) / 4;
    }

    usbdev_fifo_read(ep->buf, words);

    /* Throw away data that doesn't fit */
    if (extra) {
        usbdev_fifo_read(NULL, extra);
    }
}

void usbdev_data_in(struct endpoint *ep) {
    if (ep == NULL) {
        printk("Warning: Invalid endpoint in usbdev_data_in. ");
        return;
    }

    if (!ep->buf) {
        return;
    }

    printk("Writing FIFO %d: ", ep->num);

    int written = 0;
    int space = *USB_FS_DTXFSTS(ep->num);
    while (written < space && ep->buf_len > 0) {
        printk("0x%x ", *ep->buf);
        *USB_FS_DFIFO_EP(ep->num) = *ep->buf;
        ep->buf++;
        ep->buf_len -= 4;
        written++;
    }

    if (ep->buf_len <= 0) {
        ep->buf = NULL;
        *USB_FS_DIEPEMPMSK &= ~(1 << ep->num);

        if (ep->buf_len < 0) {
            ep->buf_len = 0;
            printk("warning: wrote past end of buffer. ");
        }
    }
}

void usbdev_status_in_packet(void) {
    *USB_FS_DIEPTSIZ0 = USB_FS_DIEPTSIZ0_PKTCNT(1) | USB_FS_DIEPTSIZ0_XFRSIZ(0);
    *USB_FS_DIEPCTL0 |= USB_FS_DIEPCTL0_CNAK | USB_FS_DIEPCTL0_EPENA;

    *USB_FS_DOEPTSIZ0 = USB_FS_DOEPTSIZ0_STUPCNT(3) | USB_FS_DOEPTSIZ0_PKTCNT(1) | USB_FS_DOEPTSIZ0_XFRSIZ(8*3);
}

void usbdev_enable_receive(struct endpoint *ep) {
    if (ep == NULL) {
        printk("Warning: Invalid endpoint in usbdev_enable_receive ");
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
