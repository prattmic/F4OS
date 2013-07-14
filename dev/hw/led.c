#include <stdint.h>
#include <dev/hw/gpio.h>
#include <dev/hw/led.h>
#include <kernel/obj.h>

struct obj *power_led;

void init_power_led(void) {
    power_led = led_get(0);
    if (!power_led) {
        return;
    }

    led_enable(power_led);
}

struct obj *led_get(uint8_t num) {
    int err;
    struct obj *obj;
    struct led led;

    if (num >= num_leds) {
        return NULL;
    }

    led = leds_avail[num];

    obj = gpio_get(led.gpio);
    if (!obj) {
        return NULL;
    }

    struct gpio *gpio = to_gpio(obj);
    struct gpio_ops *ops = gpio->obj.ops;

    err = ops->active_low(gpio, led.active_low);
    if (err) {
        goto err_put;
    }

    err = ops->direction(gpio, GPIO_OUTPUT);
    if (err) {
        goto err_put;
    }

    return obj;

err_put:
    gpio_put(obj);
    return NULL;
}

void led_put(struct obj *led) {
    gpio_put(led);
}

int led_set(struct obj *led, uint8_t value) {
    struct gpio *gpio = to_gpio(led);
    struct gpio_ops *ops = gpio->obj.ops;

    return ops->set_output_value(gpio, value);
}

int led_enable(struct obj *led) {
    return led_set(led, 1);
}

int led_disable(struct obj *led) {
    return led_set(led, 0);
}

int led_toggle(struct obj *led) {
    struct gpio *gpio = to_gpio(led);
    struct gpio_ops *ops = gpio->obj.ops;

    return ops->set_output_value(gpio, !ops->get_output_value(gpio));
}
