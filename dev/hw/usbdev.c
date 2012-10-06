#include "types.h"
#include "registers.h"
#include "interrupt.h"
#include "stdio.h"

#include "usbdev.h"

static inline void usbdev_clocks_init(void);

static inline void usbdev_handle_usbrst(void);
static inline void usbdev_handle_srqint(void);
static inline void usbdev_handle_enumdne(void);
static inline void usbdev_handle_sof(void);
static inline void usbdev_handle_rxflvl(void);

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
    *USB_FS_GINTMSK |= USB_FS_GINTMSK_MMISM | USB_FS_GINTMSK_OTGINT | USB_FS_GINTMSK_USBRSTM | USB_FS_GINTMSK_ENUMDNEM | USB_FS_GINTMSK_ESUSPM | USB_FS_GINTMSK_USBUSPM | USB_FS_GINTMSK_SOFM | USB_FS_GINTMSK_RXFLVLM | USB_FS_GINTMSK_IEPINT | USB_FS_GINTMSK_OEPINT;

    /* Set device to full speed (USB 1.1) */
    *USB_FS_DCFG |= USB_FS_DCFG_DSPD_FS | USB_FS_DCFG_NZLSOHSK;

    /* Enable VBUS B sensing */
    *USB_FS_GCCFG |= USB_FS_GCCFG_VBUSBSEN | USB_FS_GCCFG_PWRDWN;
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
        printf("\r\nSending word: 0x%x ", *packet);
        *USB_FS_DFIFO_EP0 = *packet;
        packet++;
        size -= 4;
    }

    printf("Done writing");

    while (!(*USB_FS_DIEPINT0 & USB_FS_DIEPINTx_XFRC));

    *USB_FS_DIEPINT0 &= ~USB_FS_DIEPINTx_XFRC;

    return 0;
}

/* USB OTG FS Global Interrupt Handler */
void usbdev_handler(void) {
    uint32_t interrupts = *USB_FS_GINTSTS;
    uint8_t handled = 0;

    //printf("\r\nInterrupt = 0x%x\r\n", interrupts);
    //*LED_ODR ^= (1 << 12);
    
    /* USB Reset */
    if (interrupts & USB_FS_GINTSTS_USBRST) {
        handled = 1;
        printf("USB reset\r\n");
        usbdev_handle_usbrst();
    }
    interrupts &= ~USB_FS_GINTSTS_USBRST;

    if (interrupts & USB_FS_GINTSTS_SRQINT) {
        handled = 1;
        printf("New session detected\r\n");
        usbdev_handle_srqint();
    }
    interrupts &= ~USB_FS_GINTSTS_SRQINT;

    /* Enumeration done */
    if (interrupts & USB_FS_GINTSTS_ENUMDNE) {
        handled = 1;
        printf("Enumeration done\r\n");
        usbdev_handle_enumdne();
    }
    interrupts &= ~USB_FS_GINTSTS_ENUMDNE;

    /* Start of frame token received */
    if (interrupts & USB_FS_GINTSTS_SOF) {
        handled = 1;
        //printf("Start of frame token received.\r\n");
        usbdev_handle_sof();
    }
    interrupts &= ~USB_FS_GINTSTS_SOF;

    if (interrupts & USB_FS_GINTSTS_RXFLVL) {
        handled = 1;
        printf("Received packet: ");
        usbdev_handle_rxflvl();
    }
    interrupts &= ~USB_FS_GINTSTS_RXFLVL;

    if (interrupts & USB_FS_GINTSTS_IEPINT) {
        handled = 1;
        printf("IN endpoint interrupt\r\n");
    }
    interrupts &= ~USB_FS_GINTSTS_IEPINT;

    if (interrupts & USB_FS_GINTSTS_OEPINT) {
        handled = 1;
        printf("OUT endpoint interrupt\r\n");
    }
    interrupts &= ~USB_FS_GINTSTS_OEPINT;

    //*USB_FS_GINTSTS = USB_FS_GINTSTS_EOPF;

    //if (!handled) {
    //    printf("Unhandled interrupt: 0x%x ", interrupts);
    //}
}

static inline void usbdev_handle_usbrst(void) {
    // Do nothing, clear interrupt
    *USB_FS_GINTSTS = USB_FS_GINTSTS_USBRST;
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
            puts("Global NAK: ");
            break;
        case USB_FS_GRXSTS_PKTSTS_ORX:
            puts("OUT received: ");
            usbdev_handle_out_packet_received(receive_status);
            break;
        case USB_FS_GRXSTS_PKTSTS_OCP:
            puts("OUT complete ");
            break;
        case USB_FS_GRXSTS_PKTSTS_STUPCP:
            puts("SETUP complete ");
            break;
        case USB_FS_GRXSTS_PKTSTS_STUPRX:
            puts("SETUP received: ");
            usbdev_handle_setup_packet_received(receive_status);
            break;
        default:
            printf("Unknown packet, receive status: 0x%x ", receive_status);
    }

    puts("\r\n");
}

static inline void usbdev_handle_out_packet_received(uint32_t status) {
    uint32_t word_count = USB_FS_GRXSTS_BCNT(status)/4;

    if (word_count == 0) {
        return;
    }

    uint32_t buf[word_count];

    usbdev_rx(buf, word_count);

    for (int i = 0; i < word_count; i++) {
        printf("0x%x ", buf[i]);
    }
}

static inline void usbdev_handle_setup_packet_received(uint32_t status) {
    uint32_t word_count = USB_FS_GRXSTS_BCNT(status)/4;

    if (word_count == 0) {
        return;
    }

    uint32_t buf[word_count];

    usbdev_rx(buf, word_count);

    for (int i = 0; i < word_count; i++) {
        printf("0x%x ", buf[i]);
    }

    parse_setup_packet(buf, word_count);
}

static inline void usbdev_send_status_packet(void) {
    *USB_FS_DIEPTSIZ0 = USB_FS_DIEPTSIZ0_PKTCNT(1) | USB_FS_DIEPTSIZ0_XFRSIZ(0);
    *USB_FS_DIEPCTL0 |= USB_FS_DIEPCTL0_CNAK | USB_FS_DIEPCTL0_EPENA;
}

static void parse_setup_packet(uint32_t *packet, uint32_t len) {
    struct usbdev_setup_packet *setup = (struct usbdev_setup_packet *) packet;

    switch (setup->request) {
    case USB_SETUP_REQUEST_GET_DESCRIPTOR:
        printf("GET_DESCRIPTOR ");
        switch (setup->value >> 8) {
        case USB_SETUP_DESCRIPTOR_DEVICE:
            printf("DEVICE ");
            usbdev_tx((uint32_t *) &usb_device_descriptor, sizeof(struct usb_device_descriptor));
            break;
        case USB_SETUP_DESCRIPTOR_CONFIG:
            printf("CONFIGURATION ");
            break;
        default:
            printf("OTHER DESCRIPTOR %d ", setup->value >> 8);
        }
        break;
    case USB_SETUP_REQUEST_SET_ADDRESS:
        printf("SET_ADDRESS %d ", setup->value);
        *USB_FS_DCFG |= USB_FS_DCFG_DAD(setup->value);
        usbdev_send_status_packet();
        break;
    default:
        printf("OTHER_REQUEST ");
    }
}
