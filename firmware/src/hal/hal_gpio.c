/**
 * @file hal_gpio.c
 * @brief GPIO abstraction for LED, range selection, chip select (M-07)
 */

#include "hal_gpio.h"

void hal_gpio_init(void)
{
    /* TODO: Initialize GPIO pins via MCC driver */
}

void hal_gpio_write(hal_pin_t pin, bool state)
{
    (void)pin;
    (void)state;
    /* TODO: Write to GPIO pin via MCC driver */
}

bool hal_gpio_read(hal_pin_t pin)
{
    (void)pin;
    /* TODO: Read GPIO pin via MCC driver */
    return false;
}

void hal_gpio_toggle(hal_pin_t pin)
{
    (void)pin;
    /* TODO: Toggle GPIO pin via MCC driver */
}
