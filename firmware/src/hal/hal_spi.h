/**
 * @file hal_spi.h
 * @brief SPI abstraction for MCP3204 ADC communication (M-08)
 */

#ifndef BTOHM_HAL_HAL_SPI_H_
#define BTOHM_HAL_HAL_SPI_H_

#include <stddef.h>
#include <stdint.h>

/**
 * @brief Initialize the SPI peripheral.
 */
void hal_spi_init(void);

/**
 * @brief Perform a synchronous SPI transfer.
 *
 * Asserts CS before transfer and deasserts after.
 *
 * @param tx  Pointer to transmit buffer.
 * @param rx  Pointer to receive buffer, or NULL for TX-only.
 * @param len Number of bytes to transfer.
 */
void hal_spi_transfer(const uint8_t* tx, uint8_t* rx, size_t len);

#endif  /* BTOHM_HAL_HAL_SPI_H_ */
