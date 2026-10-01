/**
 * @file hal_spi.h
 * @brief SPI abstraction for MCP3204 ADC communication (M-08)
 */

#ifndef HAL_SPI_H
#define HAL_SPI_H

#include <stdint.h>
#include <stddef.h>

void hal_spi_init(void);
void hal_spi_transfer(const uint8_t* tx, uint8_t* rx, size_t len);

#endif /* HAL_SPI_H */
