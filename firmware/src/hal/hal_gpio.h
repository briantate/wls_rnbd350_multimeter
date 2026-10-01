/**
 * @file hal_gpio.h
 * @brief GPIO abstraction for LED, range selection, chip select (M-07)
 */

#ifndef BTOHM_HAL_HAL_GPIO_H_
#define BTOHM_HAL_HAL_GPIO_H_

#include <stdbool.h>

typedef enum {
    HAL_PIN_LED,
    HAL_PIN_RES_A,
    HAL_PIN_RES_B,
    HAL_PIN_RES_C,
    HAL_PIN_SPI_CS
} hal_pin_t;

/**
 * @brief Initialize all GPIO pins.
 */
void hal_gpio_init(void);

/**
 * @brief Write a logic level to a GPIO pin.
 *
 * @param pin   Pin identifier.
 * @param state true for high, false for low.
 */
void hal_gpio_write(hal_pin_t pin, bool state);

/**
 * @brief Read the current state of a GPIO pin.
 *
 * @param pin Pin identifier.
 * @return true if high, false if low.
 */
bool hal_gpio_read(hal_pin_t pin);

/**
 * @brief Toggle a GPIO pin.
 *
 * @param pin Pin identifier.
 */
void hal_gpio_toggle(hal_pin_t pin);

#endif  /* BTOHM_HAL_HAL_GPIO_H_ */
