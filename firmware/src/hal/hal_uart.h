/**
 * @file hal_uart.h
 * @brief UART abstraction for BLE module and diagnostic output (M-09)
 */

#ifndef HAL_UART_H
#define HAL_UART_H

#include <stdint.h>
#include <stdbool.h>

typedef enum {
    HAL_UART_BLE,
    HAL_UART_DIAG
} hal_uart_channel_t;

void hal_uart_init(hal_uart_channel_t ch);
bool hal_uart_tx_ready(hal_uart_channel_t ch);
void hal_uart_tx_byte(hal_uart_channel_t ch, uint8_t byte);
void hal_uart_tx_string(hal_uart_channel_t ch, const char* str);
bool hal_uart_rx_available(hal_uart_channel_t ch);
bool hal_uart_rx_byte(hal_uart_channel_t ch, uint8_t* byte);

#endif /* HAL_UART_H */
