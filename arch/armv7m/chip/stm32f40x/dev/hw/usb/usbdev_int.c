#include <stddef.h>
#include <arch/chip/registers.h>
#include <kernel/fault.h>

#include "usbdev_internals.h"
#include "usbdev_desc.h"
#include "usbdev_class.h"
#include <dev/hw/usbdev.h>

/* Setup packet buffer */
uint8_t setup_buf[3*4];

struct ring_buffer setup_packet = {
    .buf = setup_buf,
    .len = sizeof(setup_buf)/sizeof(setup_buf[0]),
    .start = 0,
    .end = 0
};

/* Global interrupt handlers */
static void gint_mmis(void);
static void gint_otgint(void);
static void gint_sof(void);
static void gint_rxflvl(void);
/* static void gint_nptxfe(void); */
static void gint_ginakeff(void);
static void gint_gonakeff(void);
static void gint_esusp(void);
static void gint_usbsusp(void);
static void gint_usbrst(void);
static void gint_enumdne(void);
static void gint_isoodrp(void);
static void gint_eopf(void);
static void gint_iepint(void);
static void gint_oepint(void);
static void gint_iisoixfr(void);
static void gint_ipxfr(void);
static void gint_cidschg(void);
static void gint_srqint(void);
static void gint_wkupint(void);

void (*usbdev_gint_handler[32])(void) = {
    NULL,
    &gint_mmis,
    &gint_otgint,
    &gint_sof,
    &gint_rxflvl,
    NULL,   /* &gint_nptxfe, */
    &gint_ginakeff,
    &gint_gonakeff,
    NULL,
    NULL,
    &gint_esusp,
    &gint_usbsusp,
    &gint_usbrst,
    &gint_enumdne,
    &gint_isoodrp,
    &gint_eopf,
    NULL,
    NULL,
    &gint_iepint,
    &gint_oepint,
    &gint_iisoixfr,
    &gint_ipxfr,
    NULL,
    NULL,
    NULL, /* Host mode only */
    NULL, /* Host mode only */
    NULL, /* Host mode only */
    NULL,
    &gint_cidschg,
    NULL, /* Host mode only */
    &gint_srqint,
    &gint_wkupint
};

/* USB OTG FS Global Interrupt Handler */
void usbdev_handler(void) {
    uint32_t interrupts = *USB_FS_GINTSTS;

    /* Loop through all bits except bit 0, which isn't an interrupt */
    for (int i = 1; i < 32; i++) {
        if (interrupts & (1 << i) && usbdev_gint_handler[i]) {
            usbdev_gint_handler[i]();
        }
    }
}

static void gint_mmis(void) {
    /* Clear interrupt */
    *USB_FS_GINTSTS = USB_FS_GINTSTS_MMIS;

    DEBUG_PRINT("USB: Mode mismatch interrupt.  Warning: Unhandled.\r\n");
}

static void gint_otgint(void) {
    DEBUG_PRINT("USB: OTG interrupt. ");

    /* Handle */
    uint32_t interrupts = *USB_FS_GOTGINT;

    if (interrupts & USB_FS_GOTGINT_SEDET) {
        *USB_FS_GOTGINT = USB_FS_GOTGINT_SEDET;
        usb_ready = 0;
        DEBUG_PRINT("Session end detected. ");
    }
    if (interrupts & USB_FS_GOTGINT_SRSSCHG) {
        *USB_FS_GOTGINT = USB_FS_GOTGINT_SRSSCHG;
        DEBUG_PRINT("Session request status change. Warning: Unhandled. ");
    }
    if (interrupts & USB_FS_GOTGINT_HNSSCHG) {
        *USB_FS_GOTGINT = USB_FS_GOTGINT_HNSSCHG;
        DEBUG_PRINT("Host negotiation success status change. Warning: Unhandled. ");
    }
    if (interrupts & USB_FS_GOTGINT_HNGDET) {
        *USB_FS_GOTGINT = USB_FS_GOTGINT_HNGDET;
        DEBUG_PRINT("Host negotiation detected. Warning: Unhandled. ");
    }
    if (interrupts & USB_FS_GOTGINT_ADTOCHG) {
        *USB_FS_GOTGINT = USB_FS_GOTGINT_ADTOCHG;
        DEBUG_PRINT("A-device timeout change. Warning: Unhandled. ");
    }

    DEBUG_PRINT("\r\n");
}

static void gint_sof(void) {
    /* Clear interrupt */
    *USB_FS_GINTSTS = USB_FS_GINTSTS_SOF;
}

static void gint_rxflvl(void) {
    DEBUG_PRINT("USB: Received data. ");

    /* Handle */
    uint32_t receive_status = *USB_FS_GRXSTSP;

    switch (USB_FS_GRXSTS_PKTSTS(receive_status)) {
        case USB_FS_GRXSTS_PKTSTS_NAK:
            DEBUG_PRINT("Global OUT NAK.");
            break;
        case USB_FS_GRXSTS_PKTSTS_ORX:
            DEBUG_PRINT("OUT received: ");
            usbdev_data_out(receive_status);
            break;
        case USB_FS_GRXSTS_PKTSTS_OCP:
            DEBUG_PRINT("OUT complete ");
            break;
        case USB_FS_GRXSTS_PKTSTS_STUPCP:
            DEBUG_PRINT("SETUP complete ");
            usbdev_fifo_read(NULL, 4);
            break;
        case USB_FS_GRXSTS_PKTSTS_STUPRX:
            DEBUG_PRINT("SETUP received: ");
            /* This will be parsed on interrupt after SETUP complete */
            usbdev_fifo_read(&setup_packet, 8);
            break;
        default:
            DEBUG_PRINT("Error: Undefined receive status: 0x%x ", receive_status);
    }

    DEBUG_PRINT("\r\n");

    usbdev_enable_receive(endpoints[USB_FS_GRXSTS_EPNUM(receive_status)]);
}

/* static void gint_nptxfe(void) {
    DEBUG_PRINT("USB: Non-periodic TX FIFO empty.  Warning: Unhandled.\r\n");
} */

static void gint_ginakeff(void) {
    DEBUG_PRINT("USB: Global IN NAK effective.  Warning: Unhandled.\r\n");
}

static void gint_gonakeff(void) {
    DEBUG_PRINT("USB: Global OUT NAK effective.  Warning: Unhandled.\r\n");
}

static void gint_esusp(void) {
    /* Clear interrupt */
    *USB_FS_GINTSTS = USB_FS_GINTSTS_ESUSP;

    DEBUG_PRINT("USB: Early suspend. Warning: Unhandled.\r\n");
}

static void gint_usbsusp(void) {
    /* Clear interrupt */
    *USB_FS_GINTSTS = USB_FS_GINTSTS_USBSUSP;

    DEBUG_PRINT("USB: USB suspend. Warning: Unhandled.\r\n");
}

static void gint_usbrst(void) {
    /* Clear interrupt */
    *USB_FS_GINTSTS = USB_FS_GINTSTS_USBRST;

    DEBUG_PRINT("USB: USB reset.\r\n");

    /* Handle */
    usbdev_reset();
}

static void gint_enumdne(void) {
    /* Clear interrupt */
    *USB_FS_GINTSTS = USB_FS_GINTSTS_ENUMDNE;

    DEBUG_PRINT("USB: Enumeration done.\r\n");

    /* Handle */
    if ((*USB_FS_DSTS & USB_FS_DSTS_ENUMSPD) != USB_FS_DSTS_ENUMSPD_FS) {
        DEBUG_PRINT("USB: Warning: USB FS enumerated a speed other than FS.\r\n");
    }

    /* Set maximum packet size */
    *USB_FS_DIEPCTL0 &= ~(USB_FS_DIEPCTL0_MPSIZE);
    *USB_FS_DIEPCTL0 |= USB_FS_DIEPCTL0_MPSIZE_64;
    *USB_FS_DOEPCTL0 &= ~(USB_FS_DOEPCTL0_MPSIZE);
    *USB_FS_DOEPCTL0 |= USB_FS_DOEPCTL0_MPSIZE_64;
}

static void gint_isoodrp(void) {
    /* Clear interrupt */
    *USB_FS_GINTSTS = USB_FS_GINTSTS_ISOODRP;

    DEBUG_PRINT("USB: Isochronous OUT packet dropped. Warning: Unhandled.\r\n");
}

static void gint_eopf(void) {
    /* Clear interrupt */
    *USB_FS_GINTSTS = USB_FS_GINTSTS_EOPF;
}

static void gint_iepint(void) {
    DEBUG_PRINT("USB: IN endpoint interrupt. ");

    /* Handle */
    for (int i = 0; i <= 3; i++) {
        if (*USB_FS_DAINT & USB_FS_DAINT_IEPINT(i)) {
            DEBUG_PRINT("Endpoint %d. ", i);
        }
        else {
            continue;
        }

        uint32_t interrupts = *USB_FS_DIEPINT(i);

        if (interrupts & USB_FS_DIEPINTx_XFRC) {
            *USB_FS_DIEPINT(i) = USB_FS_DIEPINTx_XFRC;
            DEBUG_PRINT("Transfer complete. ");
        }
        if (interrupts & USB_FS_DIEPINTx_EPDISD) {
            *USB_FS_DIEPINT(i) = USB_FS_DIEPINTx_EPDISD;
            DEBUG_PRINT("Endpoint disabled. ");

            *USB_FS_GRSTCTL = USB_FS_GRSTCTL_TXFNUM(i) | USB_FS_GRSTCTL_TXFFLSH;

            /* Wait for FIFO to flush */
            while (*USB_FS_GRSTCTL & USB_FS_GRSTCTL_TXFFLSH);

            endpoints[i]->request_disable = 0;
        }
        if (interrupts & USB_FS_DIEPINTx_TOC) {
            *USB_FS_DIEPINT(i) = USB_FS_DIEPINTx_TOC;
            DEBUG_PRINT("Timeout condition. ");
        }
        if (interrupts & USB_FS_DIEPINTx_ITTXFE) {
            *USB_FS_DIEPINT(i) = USB_FS_DIEPINTx_ITTXFE;
            DEBUG_PRINT("IN token received when TX FIFO empty. ");
        }
        if (interrupts & USB_FS_DIEPINTx_INEPNE) {
            *USB_FS_DIEPINT(i) = USB_FS_DIEPINTx_INEPNE;
            DEBUG_PRINT("IN endpoint NAK effective. ");

            /* Disable endpoint */
            if (endpoints[i]->request_disable) {
                DEBUG_PRINT("Endpoint disable requested. ");
                if (i == 0) {
                    *USB_FS_DIEPCTL0 |= USB_FS_DIEPCTL0_EPDIS | USB_FS_DIEPCTL0_SNAK;
                }
                else {
                    *USB_FS_DIEPCTL(i) |= USB_FS_DIEPCTLx_EPDIS | USB_FS_DIEPCTLx_SNAK;
                }
            }
        }
        if (interrupts & USB_FS_DIEPINTx_TXFE) {
            *USB_FS_DIEPINT(i) = USB_FS_DIEPINTx_TXFE;
            DEBUG_PRINT("Transmit FIFO empty. ");

            /* A ternary operation in an if statement, oh yeah! */
            if ((i == 0 ? (*USB_FS_DIEPCTL0 & USB_FS_DIEPCTL0_EPENA) : (*USB_FS_DIEPCTL(i) & USB_FS_DIEPCTLx_EPENA))) {
                usbdev_data_in(endpoints[i]);
            }
        }
    }

    DEBUG_PRINT("\r\n");
}

static void gint_oepint(void) {
    DEBUG_PRINT("USB: OUT endpoint interrupt. ");

    /* Handle */
    for (int i = 0; i <= 3; i++) {
        if (*USB_FS_DAINT & USB_FS_DAINT_OEPINT(i)) {
            DEBUG_PRINT("Endpoint %d. ", i);
        }
        else {
            continue;
        }

        uint32_t interrupts = *USB_FS_DOEPINT(i);

        if (interrupts & USB_FS_DOEPINTx_XFRC) {
            *USB_FS_DOEPINT(i) = USB_FS_DOEPINTx_XFRC;
            DEBUG_PRINT("Transfer complete. ");
        }
        if (interrupts & USB_FS_DOEPINTx_EPDISD) {
            *USB_FS_DOEPINT(i) = USB_FS_DOEPINTx_EPDISD;
            DEBUG_PRINT("Endpoint disabled. ");
        }
        if (interrupts & USB_FS_DOEPINTx_STUP) {
            *USB_FS_DOEPINT(i) = USB_FS_DOEPINTx_STUP;
            DEBUG_PRINT("SETUP phase done. ");
            usbdev_setup(&setup_packet, 4);
        }
        if (interrupts & USB_FS_DOEPINTx_OTEPDIS) {
            *USB_FS_DOEPINT(i) = USB_FS_DOEPINTx_OTEPDIS;
            DEBUG_PRINT("OUT token received when endpoint disabled. ");
        }
        if (interrupts & USB_FS_DOEPINTx_B2BSTUP) {
            *USB_FS_DOEPINT(i) = USB_FS_DOEPINTx_B2BSTUP;
            DEBUG_PRINT("Back-to-back SETUP packets received.");
        }
    }

    DEBUG_PRINT("\r\n");
}

static void gint_iisoixfr(void) {
    /* Clear interrupt */
    *USB_FS_GINTSTS = USB_FS_GINTSTS_IISOIXFR;

    DEBUG_PRINT("USB: Incomplete isochronous IN transfer interrupt. Warning: Unhandled.\r\n");
}

static void gint_ipxfr(void) {
    /* Clear interrupt */
    *USB_FS_GINTSTS = USB_FS_GINTSTS_IPXFR;

    DEBUG_PRINT("USB: Incomplete periodic transfer interrupt. Warning: Unhandled.\r\n");
}

static void gint_cidschg(void) {
    /* Clear interrupt */
    *USB_FS_GINTSTS = USB_FS_GINTSTS_CIDSCHG;

    DEBUG_PRINT("USB: Core ID change interrupt. Warning: Unhandled.\r\n");
}

static void gint_srqint(void) {
    /* Clear interrupt */
    *USB_FS_GINTSTS = USB_FS_GINTSTS_SRQINT;

    DEBUG_PRINT("USB: New session detected.\r\n");

    /* Handle */
}

static void gint_wkupint(void) {
    /* Clear interrupt */
    *USB_FS_GINTSTS = USB_FS_GINTSTS_WKUPINT;

    DEBUG_PRINT("USB: Remote wakeup interrupt. Warning: Unhandled.\r\n");
}
