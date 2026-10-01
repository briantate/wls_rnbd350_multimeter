/**
 * @file measurement_svc.c
 * @brief Resistance measurement acquisition and conversion (M-03)
 */

#include "measurement_svc.h"

#include <stdbool.h>
#include <stdint.h>

#include "../hal/hal_gpio.h"
#include "../hal/hal_spi.h"

void measurement_svc_init(void)
{
    hal_gpio_write(HAL_PIN_RES_A, false);
    hal_gpio_write(HAL_PIN_RES_B, false);
    hal_gpio_write(HAL_PIN_RES_C, false);
}

measurement_t measurement_svc_sample(void)
{
    uint8_t tx[3] = {0x06, 0x00, 0x00};
    uint8_t rx[3] = {0};

    hal_spi_transfer(tx, rx, 3);

    uint16_t counts = ((rx[1] & 0x0F) << 8) | rx[2];
    uint32_t ohms = (counts * 10000U) / 4095U;

    measurement_t result = {ohms, true};
    return result;
}
