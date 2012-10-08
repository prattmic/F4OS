#include <stdint.h>
#include <stdio.h>
#include <dev/registers.h>
#include <kernel/fault.h>

#include <dev/hw/usbdev.h>

static inline void usbdev_clocks_init(void);

static inline void usbdev_enable_receive(void);

static inline void usbdev_handle_usbrst(void);
static inline void usbdev_handle_srqint(void);
static inline void usbdev_handle_enumdne(void);
static inline void usbdev_handle_sof(void);
static inline void usbdev_handle_rxflvl(void);
static inline void usbdev_handle_oepint(void);

static inline void usbdev_send_status_packet(void);

static inline void usbdev_handle_out_packet_received(uint32_t status);
static inline void usbdev_handle_setup_packet_received(uint32_t status);

static void parse_setup_packet(uint32_t *packet, uint32_t len);

const struct usb_device_descriptor {
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
} usb_device_descriptor = {
    sizeof(struct usb_device_descriptor),
    1,
    0x110,
    0x02,
    0x02,
    0,
    64,
    0x0483,     /* https://github.com/ericherman/stm32f4-discovery-example/blob/master/rotate-chars/rotate-chars-usb-descriptors.h */
    0x5740,
    1,
    0,
    0,
    0,
    1
};

static inline void usbdev_clocks_init(void) {
    *RCC_AHB2ENR |= RCC_AHB2ENR_OTGFSEN;    /* Enable USB OTG FS clock */
    *RCC_AHB1ENR |= RCC_AHB1ENR_GPIOAEN;    /* Enable GPIOA Clock */
    *NVIC_ISER2 |= (1 << 3);                /* Unable USB FS Interrupt */

    /* Set PA9, PA10, PA11, and PA12 to alternative function OTG
     * See stm32f4_ref.pdf pg 141 and stm32f407.pdf pg 51 */

    /* Sets PA8, PA9, PA10, PA11, and PA12 to alternative function mode */
    *GPIOA_MODER &= ~((3 << (8 * 2)) | (3 << (9 * 2)) | (3 << (10 * 2)) | (3 << (11 * 2)) | (3 << (12 * 2)));
    *GPIOA_MODER |= (GPIO_MODER_ALT << (8 * 2)) | (GPIO_MODER_ALT << (9 * 2)) | (GPIO_MODER_ALT << (10 * 2)) | (GPIO_MODER_ALT << (11 * 2)) | (GPIO_MODER_ALT << (12 * 2));

    /* Sets PA8, PA9, PA10, PA11, and PA12 to OTG mode */
    *GPIOA_AFRH  &= ~((0xF << ((8-8) * 4)) | (0xF << ((9-8) * 4)) | (0xF << ((10-8) * 4)) | (0xF << ((11-8) * 4)) | (0xF << ((12-8) * 4)));
    *GPIOA_AFRH  |= (GPIO_AF_OTG << ((8-8) * 4)) | (GPIO_AF_OTG << ((9-8) * 4)) | (GPIO_AF_OTG << ((10-8) * 4)) | (GPIO_AF_OTG << ((11-8) * 4)) | (GPIO_AF_OTG << ((12-8) * 4));

    /* Sets pin output to push/pull */
    *GPIOA_OTYPER &= ~((1 << 8) | (1 << 9) | (1 << 11) | (1 << 12));
    *GPIOA_OTYPER |= (1 << 10);

    /* No pull-up, no pull-down */
    *GPIOA_PUPDR &= ~((3 << (8 * 2)) | (3 << (9 * 2)) | (3 << (10 * 2)) | (3 << (11 * 2)) | (3 << (12 * 2)));
    *GPIOA_PUPDR |= (1 << (10 * 2));

    /* Speed to 100Mhz */
    *GPIOA_OSPEEDR &= ~((3 << (8 * 2)) | (3 << (9 * 2)) | (3 << (10 * 2)) | (3 << (11 * 2)) | (3 << (12 * 2)));
    *GPIOA_OSPEEDR |= (3 << (8 * 2)) | (3 << (9 * 2)) | (3 << (10 * 2)) | (3 << (11 * 2)) | (3 << (12 * 2));
}

void init_usbdev(void) {
    usbdev_clocks_init();

    /* Global unmask of USB interrupts, TX empty interrupt when TX is actually empty */
    *USB_FS_GAHBCFG |= USB_FS_GAHBCFG_GINTMSK | USB_FS_GAHBCFG_TXFELVL;

    /* Enable session request protocol, USB turnaround time 15, force device mode */
    *USB_FS_GUSBCFG |= USB_FS_GUSBCFG_SRPCAP | USB_FS_GUSBCFG_TRDT(15) | USB_FS_GUSBCFG_FDMOD;

    /* Clear global interrupts */
    *USB_FS_GINTSTS = 0xFFFFFFFF;
    *USB_FS_GOTGINT = 0xFFFFFFFF;

    /* Unmask mode mismatch, OTG, USB reset, enum done, early susp, USB susp, and SOF interrupts */
    *USB_FS_GINTMSK |= USB_FS_GINTMSK_USBRSTM | USB_FS_GINTMSK_ENUMDNEM | USB_FS_GINTMSK_ESUSPM | USB_FS_GINTMSK_USBSUSPM | USB_FS_GINTMSK_SOFM;

    /* Set device to full speed (USB 1.1) */
    *USB_FS_DCFG |= USB_FS_DCFG_DSPD_FS | USB_FS_DCFG_NZLSOHSK;

    /* Enable VBUS B sensing */
    *USB_FS_GCCFG |= USB_FS_GCCFG_VBUSBSEN | USB_FS_GCCFG_PWRDWN;
}

static inline void usbdev_enable_receive(void) {
    *USB_FS_DOEPCTL0 |= USB_FS_DOEPCTL0_CNAK;
}

void usbdev_rx(uint32_t *buf, int words) {
    while (words > 0) {
        *buf = *USB_FS_DFIFO_EP0;
        buf++;
        words--;
    }
}

/* packet points to first word in packet.  size is packet size in bytes */
int usbdev_tx(uint32_t *packet, int size) {
    *USB_FS_DIEPTSIZ0 = USB_FS_DIEPTSIZ0_PKTCNT(1) | USB_FS_DIEPTSIZ0_XFRSIZ(size);

    *USB_FS_DIEPCTL0 |= USB_FS_DIEPCTL0_CNAK | USB_FS_DIEPCTL0_EPENA;

    while (size > 0) {
        while (!*USB_FS_DTXFSTS0);

        printk("\r\nSending word: 0x%x ", *packet);
        *USB_FS_DFIFO_EP0 = *packet;
        packet++;
        size -= 4;
    }

    printk("Done writing ");

    while (!(*USB_FS_DIEPINT0 & USB_FS_DIEPINTx_XFRC));

    *USB_FS_DIEPINT0 = USB_FS_DIEPINTx_XFRC;

    printk("Transfer complete");

    return 0;
}

/* USB OTG FS Global Interrupt Handler */
void usbdev_handler(void) {
    uint32_t interrupts = *USB_FS_GINTSTS;
    uint8_t handled = 0;
    
    // Clear interrupts
    *USB_FS_GINTSTS = 0xFFFFFFFF;

    //printk("\r\nInterrupt = 0x%x\r\n", interrupts);
    //*LED_ODR ^= (1 << 12);
    
    /* USB Reset */
    if (interrupts & USB_FS_GINTSTS_USBRST) {
        handled = 1;
        printk("USB: Reset\r\n");
        usbdev_handle_usbrst();
    }
    interrupts &= ~USB_FS_GINTSTS_USBRST;

    if (interrupts & USB_FS_GINTSTS_SRQINT) {
        handled = 1;
        printk("USB: New session detected\r\n");
        usbdev_handle_srqint();
    }
    interrupts &= ~USB_FS_GINTSTS_SRQINT;

    /* Enumeration done */
    if (interrupts & USB_FS_GINTSTS_ENUMDNE) {
        handled = 1;
        printk("USB: Enumeration done\r\n");
        usbdev_handle_enumdne();
    }
    interrupts &= ~USB_FS_GINTSTS_ENUMDNE;

    /* Start of frame token received */
    if (interrupts & USB_FS_GINTSTS_SOF) {
        handled = 1;
        //printk("Start of frame token received.\r\n");
        usbdev_handle_sof();
    }
    interrupts &= ~USB_FS_GINTSTS_SOF;

    if (interrupts & USB_FS_GINTSTS_RXFLVL) {
        handled = 1;
        printk("USB: Received packet: ");
        usbdev_handle_rxflvl();
        usbdev_enable_receive();
    }
    interrupts &= ~USB_FS_GINTSTS_RXFLVL;

    if (interrupts & USB_FS_GINTSTS_IEPINT) {
        handled = 1;
        printk("USB: IN endpoint interrupt\r\n");
    }
    interrupts &= ~USB_FS_GINTSTS_IEPINT;

    if (interrupts & USB_FS_GINTSTS_OEPINT) {
        handled = 1;
        printk("USB: OUT endpoint interrupt. ");
        usbdev_handle_oepint();
    }
    interrupts &= ~USB_FS_GINTSTS_OEPINT;

    if (interrupts & USB_FS_GINTSTS_EOPF) {
        handled = 1;
        //printk("End of periodic frame\r\n");
    }
    interrupts &= ~USB_FS_GINTSTS_EOPF;

    if (interrupts & USB_FS_GINTSTS_ESUSP) {
        handled = 1;
        printk("USB: Early suspend.\r\n");
    }
    interrupts &= ~USB_FS_GINTSTS_ESUSP;

    if (interrupts & USB_FS_GINTSTS_USBSUSP) {
        handled = 1;
        printk("USB: USB suspend.\r\n");
    }
    interrupts &= ~USB_FS_GINTSTS_USBSUSP;

    if (interrupts & USB_FS_GINTSTS_OTGINT) {
        handled = 1;
        printk("USB: OTG Interrupt: 0x%x.\r\n", *USB_FS_GOTGINT);
        *USB_FS_GOTGINT = 0xFFFFFFFF;
    }
    interrupts &= ~USB_FS_GINTSTS_OTGINT;

    if (!handled) {
        printk("USB: Unhandled interrupt: 0x%x \r\n", interrupts);
    }
}

#define RX_FIFO_SIZE    128
#define TX0_FIFO_SIZE   64
#define TX1_FIFO_SIZE   128
#define TX2_FIFO_SIZE   0
#define TX3_FIFO_SIZE   0

static inline void usbdev_handle_usbrst(void) {
    // Clear interrupt
    *USB_FS_GINTSTS = USB_FS_GINTSTS_USBRST;

    /* NAK bits */
    *USB_FS_DOEPCTL0 |= USB_FS_DOEPCTL0_SNAK;
    *USB_FS_DOEPCTL1 |= USB_FS_DOEPCTLx_SNAK;
    *USB_FS_DOEPCTL2 |= USB_FS_DOEPCTLx_SNAK;
    *USB_FS_DOEPCTL3 |= USB_FS_DOEPCTLx_SNAK;

    /* Unmask interrupts */
    *USB_FS_DAINTMSK |= USB_FS_DAINT_IEPM(0) | USB_FS_DAINT_OEPM(0);
    *USB_FS_DOEPMSK |= USB_FS_DOEPMSK_STUPM | USB_FS_DOEPMSK_XFRCM;

    /* Back-to-back SETUP packets */
    *USB_FS_DOEPTSIZ0 |= USB_FS_DOEPTSIZ0_STUPCNT(3);

    /* Set RX FIFO size */
    *USB_FS_GRXFSIZ = RX_FIFO_SIZE;

    /* EP0 TX Size */
    *USB_FS_DIEPTXF0 = USB_FS_DIEPTXF0_TX0FSA(RX_FIFO_SIZE) | USB_FS_DIEPTXF0_TX0FD(TX0_FIFO_SIZE);
    /* EP1 TX Size */
    *USB_FS_DIEPTXF1 = USB_FS_DIEPTXF_INEPTXSA((RX_FIFO_SIZE+TX0_FIFO_SIZE)) | USB_FS_DIEPTXF_INEPTXFD(TX1_FIFO_SIZE);
    /* EP2 TX Size */
    *USB_FS_DIEPTXF2 = USB_FS_DIEPTXF_INEPTXSA((RX_FIFO_SIZE+TX0_FIFO_SIZE+TX1_FIFO_SIZE)) | USB_FS_DIEPTXF_INEPTXFD(TX2_FIFO_SIZE);
    /* EP3 TX Size */
    *USB_FS_DIEPTXF3 = USB_FS_DIEPTXF_INEPTXSA((RX_FIFO_SIZE+TX0_FIFO_SIZE+TX1_FIFO_SIZE+TX2_FIFO_SIZE)) | USB_FS_DIEPTXF_INEPTXFD(TX3_FIFO_SIZE);
}

static inline void usbdev_handle_srqint(void) {
    // Do nothing, clear interrupt
    *USB_FS_GINTSTS = USB_FS_GINTSTS_SRQINT;
}

static inline void usbdev_handle_enumdne(void) {
    uint32_t device_status = *USB_FS_DSTS;

    if ((device_status & USB_FS_DSTS_ENUMSPD) != USB_FS_DSTS_ENUMSPD_FS) {
        panic_print("USB FS enumerated a speed other than FS.");
    }

    /* Set maximum packet size */
    *USB_FS_DIEPCTL0 &= ~(USB_FS_DIEPCTL0_MPSIZ);
    *USB_FS_DIEPCTL0 |= USB_FS_DIEPCTL0_MPSIZ_64;

    /* Clear interrupt */
    *USB_FS_GINTSTS = USB_FS_GINTSTS_ENUMDNE;
}

static inline void usbdev_handle_sof(void) {
    /* Clear interrupt */
    *USB_FS_GINTSTS = USB_FS_GINTSTS_SOF;
}

static inline void usbdev_handle_rxflvl(void) {
    uint32_t receive_status = *USB_FS_GRXSTSP;
    uint32_t packet_status = USB_FS_GRXSTS_PKTSTS(receive_status);

    switch (packet_status) {
        case USB_FS_GRXSTS_PKTSTS_NAK:
            printk("Global NAK: ");
            break;
        case USB_FS_GRXSTS_PKTSTS_ORX:
            printk("OUT received: ");
            usbdev_handle_out_packet_received(receive_status);
            break;
        case USB_FS_GRXSTS_PKTSTS_OCP:
            printk("OUT complete ");
            break;
        case USB_FS_GRXSTS_PKTSTS_STUPCP:
            printk("SETUP complete ");
            uint32_t buffer;
            usbdev_rx(&buffer, sizeof(buffer)/4);
            break;
        case USB_FS_GRXSTS_PKTSTS_STUPRX:
            printk("SETUP received: ");
            usbdev_handle_setup_packet_received(receive_status);
            break;
        default:
            printk("Unknown packet, receive status: 0x%x ", receive_status);
    }

    printk("\r\n");
}

static inline void usbdev_handle_out_packet_received(uint32_t status) {
    uint32_t word_count = USB_FS_GRXSTS_BCNT(status)/4;

    if (word_count == 0) {
        return;
    }

    uint32_t buf[word_count];

    usbdev_rx(buf, word_count);

    for (int i = 0; i < word_count; i++) {
        printk("0x%x ", buf[i]);
    }
}

uint32_t setup_packet[2];

static inline void usbdev_handle_setup_packet_received(uint32_t status) {
    uint32_t word_count = USB_FS_GRXSTS_BCNT(status)/4;

    if (word_count == 0) {
        return;
    }

    //uint32_t buf[word_count];

    //usbdev_rx(buf, word_count);
    usbdev_rx(setup_packet, 2);

    for (int i = 0; i < word_count; i++) {
        //printk("0x%x ", buf[i]);
        printk("0x%x ", setup_packet[i]);
    }

    //parse_setup_packet(buf, word_count);
}

static inline void usbdev_send_status_packet(void) {
    *USB_FS_DIEPTSIZ0 = USB_FS_DIEPTSIZ0_PKTCNT(1) | USB_FS_DIEPTSIZ0_XFRSIZ(0);
    *USB_FS_DIEPCTL0 |= USB_FS_DIEPCTL0_CNAK | USB_FS_DIEPCTL0_EPENA;

    *USB_FS_DOEPTSIZ0 = USB_FS_DOEPTSIZ0_STUPCNT(3) | USB_FS_DOEPTSIZ0_PKTCNT(1) | USB_FS_DOEPTSIZ0_XFRSIZ(8*3);
    //*USB_FS_DOEPCTL0 |= USB_FS_DOEPCTL0_CNAK | USB_FS_DOEPCTL0_EPENA;
}

static void parse_setup_packet(uint32_t *packet, uint32_t len) {
    struct usbdev_setup_packet *setup = (struct usbdev_setup_packet *) packet;

    switch (setup->request) {
    case USB_SETUP_REQUEST_GET_DESCRIPTOR:
        printk("GET_DESCRIPTOR ");
        switch (setup->value >> 8) {
        case USB_SETUP_DESCRIPTOR_DEVICE:
            printk("DEVICE ");
            usbdev_tx((uint32_t *) &usb_device_descriptor, sizeof(struct usb_device_descriptor));
            break;
        case USB_SETUP_DESCRIPTOR_CONFIG:
            printk("CONFIGURATION ");
            break;
        default:
            printk("OTHER DESCRIPTOR %d ", setup->value >> 8);
        }
        break;
    case USB_SETUP_REQUEST_SET_ADDRESS:
        printk("SET_ADDRESS %d ", setup->value);
        *USB_FS_DCFG |= USB_FS_DCFG_DAD(setup->value);
        usbdev_send_status_packet();
        break;
    case USB_SETUP_REQUEST_GET_STATUS:
        printk("GET_STATUS ");
        if (setup->recipient == USB_SETUP_REQUEST_TYPE_RECIPIENT_DEVICE) {
            printk("DEVICE ");
            uint32_t buf = 0x11; /* Self powered and remote wakeup */
            usbdev_tx(&buf, sizeof(buf));
        }
        else {
            printk("OTHER ");
        }
        break;
    default:
        printk("OTHER_REQUEST ");
    }
}

static inline void usbdev_handle_oepint(void) {
    if (*USB_FS_DAINT & (1 << 16)) {
        printk("On endpoint 0. ");
    }
    else {
        printk("Not on endpoint 0.\r\n");
        return;
    }

    uint32_t interrupts = *USB_FS_DOEPINT0;

    if (interrupts & USB_FS_DOEPINTx_XFRC) {
        *USB_FS_DOEPINT0 = USB_FS_DOEPINTx_XFRC;
        printk("Transfer complete. ");
    }

    if (interrupts & USB_FS_DOEPINTx_EPDISD) {
        *USB_FS_DOEPINT0 = USB_FS_DOEPINTx_EPDISD;
        printk("Endpoint disabled. ");
    }

    if (interrupts & USB_FS_DOEPINTx_STUP) {
        *USB_FS_DOEPINT0 = USB_FS_DOEPINTx_STUP;
        printk("SETUP phase done. ");
        parse_setup_packet(setup_packet, 2);
        //usbdev_handle_rxflvl();
    }

    if (interrupts & USB_FS_DOEPINTx_OTEPDIS) {
        *USB_FS_DOEPINT0 = USB_FS_DOEPINTx_OTEPDIS;
        printk("OUT token received when endpoint disabled. ");
    }

    if (interrupts & USB_FS_DOEPINTx_B2BSTUP) {
        *USB_FS_DOEPINT0 = USB_FS_DOEPINTx_B2BSTUP;
        printk("Back-to-back SETUP packets received.");
    }

    printk("\r\n");

    /* Clear interrupt */
    *USB_FS_GINTSTS = USB_FS_GINTSTS_OEPINT;
}
