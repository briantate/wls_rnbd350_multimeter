/**
 * @file hal_gpio.h
 * @brief GPIO abstraction for LED, range selection, chip select (M-07)
 */

#ifndef HAL_GPIO_H
#define HAL_GPIO_H

#include <stdbool.h>

typedef enum {
    HAL_PIN_LED,
    HAL_PIN_RES_A,
    HAL_PIN_RES_B,
    HAL_PIN_RES_C,
    HAL_PIN_SPI_CS
} hal_pin_t;

void hal_gpio_init(void);
void hal_gpio_write(hal_pin_t pin, bool state);
bool hal_gpio_read(hal_pin_t pin);
void hal_gpio_toggle(hal_pin_t pin);

#endif /* HAL_GPIO_H */
