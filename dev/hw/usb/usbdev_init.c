#include <dev/registers.h>

#include "usbdev_internals.h"
#include "usbdev_desc.h"
#include <dev/hw/usbdev.h>

static inline void usbdev_clocks_init(void);

void init_usbdev(void) {
    usbdev_clocks_init();

    /* Global unmask of USB interrupts, TX empty interrupt when TX is actually empty */
    *USB_FS_GAHBCFG |= USB_FS_GAHBCFG_GINTMSK;

    /* Enable session request protocol, USB turnaround time 15, force device mode */
    *USB_FS_GUSBCFG |= USB_FS_GUSBCFG_SRPCAP | USB_FS_GUSBCFG_TRDT(15) | USB_FS_GUSBCFG_FDMOD;

    /* Clear global interrupts */
    *USB_FS_GINTSTS = 0xFFFFFFFF;
    *USB_FS_GOTGINT = 0xFFFFFFFF;

    /* Unmask mode mismatch, OTG, USB reset, enum done, early susp, USB susp, and SOF interrupts */
    *USB_FS_GINTMSK |= USB_FS_GINTMSK_USBRSTM | USB_FS_GINTMSK_ENUMDNEM | USB_FS_GINTMSK_ESUSPM | USB_FS_GINTMSK_USBSUSPM | USB_FS_GINTMSK_SOFM;

    /* Set device to full speed (USB 1.1) */
    *USB_FS_DCFG |= USB_FS_DCFG_DSPD_FS | USB_FS_DCFG_NZLSOHSK;

    /* Enable VBUS B sensing and power on USB */
    *USB_FS_GCCFG |= USB_FS_GCCFG_VBUSBSEN | USB_FS_GCCFG_PWRDWN;
}

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

void usbdev_reset(void) {
    /* NAK bits */
    *USB_FS_DOEPCTL0 |= USB_FS_DOEPCTL0_SNAK;
    *USB_FS_DOEPCTL(1) |= USB_FS_DOEPCTLx_SNAK;
    *USB_FS_DOEPCTL(2) |= USB_FS_DOEPCTLx_SNAK;
    *USB_FS_DOEPCTL(3) |= USB_FS_DOEPCTLx_SNAK;

    /* Unmask interrupts */
    *USB_FS_DAINTMSK |= USB_FS_DAINT_IEPM(0) | USB_FS_DAINT_OEPM(0);
    *USB_FS_DOEPMSK |= USB_FS_DOEPMSK_STUPM | USB_FS_DOEPMSK_XFRCM;

    /* Back-to-back SETUP packets */
    *USB_FS_DOEPTSIZ0 |= USB_FS_DOEPTSIZ0_STUPCNT(3);

    endpoints[0] = &ep_ctl;

    /* Set RX FIFO size */
    *USB_FS_GRXFSIZ = USB_RX_FIFO_SIZE;

    /* EP0 TX Size */
    *USB_FS_DIEPTXF0 = USB_FS_DIEPTXF0_TX0FSA(USB_RX_FIFO_SIZE) | USB_FS_DIEPTXF0_TX0FD(USB_TX0_FIFO_SIZE);
    /* EP1 TX Size */
    *USB_FS_DIEPTXF1 = USB_FS_DIEPTXF_INEPTXSA((USB_RX_FIFO_SIZE+USB_TX0_FIFO_SIZE)) | USB_FS_DIEPTXF_INEPTXFD(USB_TX1_FIFO_SIZE);
    /* EP2 TX Size */
    *USB_FS_DIEPTXF2 = USB_FS_DIEPTXF_INEPTXSA((USB_RX_FIFO_SIZE+USB_TX0_FIFO_SIZE+USB_TX1_FIFO_SIZE)) | USB_FS_DIEPTXF_INEPTXFD(USB_TX2_FIFO_SIZE);
    /* EP3 TX Size */
    *USB_FS_DIEPTXF3 = USB_FS_DIEPTXF_INEPTXSA((USB_RX_FIFO_SIZE+USB_TX0_FIFO_SIZE+USB_TX1_FIFO_SIZE+USB_TX2_FIFO_SIZE)) | USB_FS_DIEPTXF_INEPTXFD(USB_TX3_FIFO_SIZE);
}
