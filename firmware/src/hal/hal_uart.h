/**
 * @file hal_uart.h
 * @brief UART abstraction for BLE module and diagnostic output (M-09)
 */

#ifndef BTOHM_HAL_HAL_UART_H_
#define BTOHM_HAL_HAL_UART_H_

#include <stdbool.h>
#include <stdint.h>

typedef enum {
    HAL_UART_BLE,
    HAL_UART_DIAG
} hal_uart_channel_t;

/**
 * @brief Initialize a UART channel.
 *
 * @param ch Channel to initialize.
 */
void hal_uart_init(hal_uart_channel_t ch);

/**
 * @brief Check if TX buffer is ready for data.
 *
 * @param ch Channel to check.
 * @return true if ready, false if busy.
 */
bool hal_uart_tx_ready(hal_uart_channel_t ch);

/**
 * @brief Transmit a single byte.
 *
 * @param ch   Channel to transmit on.
 * @param byte Byte to transmit.
 */
void hal_uart_tx_byte(hal_uart_channel_t ch, uint8_t byte);

/**
 * @brief Transmit a null-terminated string.
 *
 * @param ch  Channel to transmit on.
 * @param str String to transmit.
 */
void hal_uart_tx_string(hal_uart_channel_t ch, const char* str);

/**
 * @brief Check if RX data is available.
 *
 * @param ch Channel to check.
 * @return true if data available, false otherwise.
 */
bool hal_uart_rx_available(hal_uart_channel_t ch);

/**
 * @brief Receive a single byte.
 *
 * @param ch       Channel to receive from.
 * @param out_byte Pointer to store received byte.
 * @return true if byte received, false if buffer empty.
 */
bool hal_uart_rx_byte(hal_uart_channel_t ch, uint8_t* out_byte);

#endif  /* BTOHM_HAL_HAL_UART_H_ */
