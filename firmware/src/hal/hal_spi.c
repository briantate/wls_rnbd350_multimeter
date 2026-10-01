/**
 * @file hal_spi.c
 * @brief SPI abstraction for MCP3204 ADC communication (M-08)
 */

#include "hal_spi.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "hal_gpio.h"

void hal_spi_init(void)
{
    /* TODO: Initialize SPI peripheral via MCC driver */
}

void hal_spi_transfer(const uint8_t* tx, uint8_t* rx, size_t len)
{
    if (len == 0) {
        return;
    }

    hal_gpio_write(HAL_PIN_SPI_CS, false);
    /* TODO: Perform SPI transfer via MCC driver */
    (void)tx;
    (void)rx;
    hal_gpio_write(HAL_PIN_SPI_CS, true);
}
