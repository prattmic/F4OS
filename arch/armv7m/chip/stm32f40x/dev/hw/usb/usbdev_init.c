#include <stddef.h>
#include <stdlib.h>
#include <arch/system.h>
#include <arch/chip/gpio.h>
#include <arch/chip/registers.h>
#include <kernel/fault.h>

#include "usbdev_internals.h"
#include "usbdev_desc.h"
#include <dev/hw/usbdev.h>

static inline void usbdev_clocks_init(void);

void init_usbdev(void) {
    usbdev_clocks_init();

    ep_tx_buf[0] = malloc(4*USB_TX0_FIFO_SIZE + 1);
    ep_tx_buf[1] = malloc(4*USB_TX1_FIFO_SIZE);
    ep_tx_buf[2] = malloc(4*USB_TX2_FIFO_SIZE);
    ep_tx_buf[3] = malloc(4*USB_TX3_FIFO_SIZE);
    for (int i = 0; i < 4; i++) {
        if (ep_tx_buf[i] == NULL) {
            panic_print("USB: unable to malloc buffer.");
        }
    }

    ep_ctl.tx.buf = ep_tx_buf[0];
    ep_ctl.tx.len = 4*USB_TX0_FIFO_SIZE + 1;
    ep_ctl.tx.start = 0;
    ep_ctl.tx.end = 0;

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

    /* PA8 */
    gpio_moder(GPIOA, 8, GPIO_MODER_ALT);
    gpio_afr(GPIOA, 8, GPIO_AF_OTG);
    gpio_otyper(GPIOA, 8, GPIO_OTYPER_PP);
    gpio_pupdr(GPIOA, 8, GPIO_PUPDR_NONE);
    gpio_ospeedr(GPIOA, 8, GPIO_OSPEEDR_100M);

    /* PA9 */
    gpio_moder(GPIOA, 9, GPIO_MODER_ALT);
    gpio_afr(GPIOA, 9, GPIO_AF_OTG);
    gpio_otyper(GPIOA, 9, GPIO_OTYPER_PP);
    gpio_pupdr(GPIOA, 9, GPIO_PUPDR_NONE);
    gpio_ospeedr(GPIOA, 9, GPIO_OSPEEDR_100M);

    /* PA10 */
    gpio_moder(GPIOA, 10, GPIO_MODER_ALT);
    gpio_afr(GPIOA, 10, GPIO_AF_OTG);
    gpio_otyper(GPIOA, 10, GPIO_OTYPER_OD);
    gpio_pupdr(GPIOA, 10, GPIO_PUPDR_UP);
    gpio_ospeedr(GPIOA, 10, GPIO_OSPEEDR_100M);

    /* PA11 */
    gpio_moder(GPIOA, 11, GPIO_MODER_ALT);
    gpio_afr(GPIOA, 11, GPIO_AF_OTG);
    gpio_otyper(GPIOA, 11, GPIO_OTYPER_PP);
    gpio_pupdr(GPIOA, 11, GPIO_PUPDR_NONE);
    gpio_ospeedr(GPIOA, 11, GPIO_OSPEEDR_100M);

    /* PA12 */
    gpio_moder(GPIOA, 12, GPIO_MODER_ALT);
    gpio_afr(GPIOA, 12, GPIO_AF_OTG);
    gpio_otyper(GPIOA, 12, GPIO_OTYPER_PP);
    gpio_pupdr(GPIOA, 12, GPIO_PUPDR_NONE);
    gpio_ospeedr(GPIOA, 12, GPIO_OSPEEDR_100M);
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
