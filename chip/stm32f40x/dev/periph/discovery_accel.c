#include <stddef.h>
#include <stdlib.h>
#include <mm/mm.h>
#include <kernel/semaphore.h>
#include <kernel/sched.h>
#include <kernel/fault.h>
#include <dev/resource.h>
#include <dev/registers.h>

#include <dev/hw/spi.h>
#include <dev/periph/discovery_accel.h>

typedef struct discovery_accel {
        spi_dev *spi_port;
        uint8_t read_ctr;
} discovery_accel;

extern spi_dev spi1;

char discovery_accel_read(void *env) __attribute__((section(".kernel")));
void discovery_accel_write(char d, void *env) __attribute__((section(".kernel")));
void discovery_accel_close(resource *env) __attribute__((section(".kernel")));
static void discovery_accel_cs_high(void) __attribute__((section(".kernel")));
static void discovery_accel_cs_low(void) __attribute__((section(".kernel")));

static void discovery_accel_cs_high(void) {
        *GPIOE_ODR |= GPIO_ODR_PIN(3);
}

static void discovery_accel_cs_low(void) {
        *GPIOE_ODR &= ~(GPIO_ODR_PIN(3));
}

rd_t open_discovery_accel(void) {
    /* --- Set up CS pin and set high ---*/
    *RCC_AHB1ENR |= RCC_AHB1ENR_GPIOEEN;

    /* ------- Set PE3 to output mode */
    *GPIOE_MODER &= ~(GPIO_MODER_M(3));
    *GPIOE_MODER |= (GPIO_MODER_OUT << GPIO_MODER_PIN(3));

    /* ------ Set pin output to push/pull */
    *GPIOE_OTYPER &= ~(GPIO_OTYPER_M(3));
    *GPIOE_OTYPER |= (GPIO_OTYPER_PP << GPIO_OTYPER_PIN(3));

    /* ------ Set no pull up/down */
    *GPIOE_PUPDR &= ~(GPIO_PUPDR_M(3));
    *GPIOE_PUPDR |= (GPIO_PUPDR_NONE << GPIO_PUPDR_PIN(3));
    
    /* ------ Properly idle CS */
    discovery_accel_cs_high();

    discovery_accel *accel = kmalloc(sizeof(discovery_accel));
    resource *new_r = create_new_resource();
    if(!new_r || !accel) {
        panic_print("Could not allocate space for discovery accel resource");
    }
    /* We expect that spi1 was init'd in bootmain.c */
    accel->spi_port = &spi1;
    accel->spi_port->write(0x20, 0x47, &discovery_accel_cs_high, &discovery_accel_cs_low);
    accel->read_ctr = 0;

    new_r->env = accel;
    new_r->writer = &discovery_accel_write;
    new_r->reader = &discovery_accel_read;
    new_r->closer = &discovery_accel_close;
    new_r->sem = &spi1_semaphore;

    add_resource(curr_task->task, new_r);
    return curr_task->task->top_rd - 1;
}

char discovery_accel_read(void *env) {
    discovery_accel *accel = (discovery_accel *)env;
    if(accel->read_ctr > 2)
        accel->read_ctr = 0;
    return (char)accel->spi_port->read(0x29 + 2*accel->read_ctr++, &discovery_accel_cs_high, &discovery_accel_cs_low);
}

void discovery_accel_write(char d, void *env) {
    /* No real meaning to this yet */
}

void discovery_accel_close(resource *res) {
    kfree(res->env);
}
