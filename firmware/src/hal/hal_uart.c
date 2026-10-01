/**
 * @file hal_uart.c
 * @brief UART abstraction for BLE module and diagnostic output (M-09)
 */

#include "hal_uart.h"

#include <stdbool.h>
#include <stdint.h>

void hal_uart_init(hal_uart_channel_t ch)
{
    (void)ch;
    /* TODO: Initialize UART peripheral via MCC driver */
}

bool hal_uart_tx_ready(hal_uart_channel_t ch)
{
    (void)ch;
    /* TODO: Check TX ready via MCC driver */
    return true;
}

void hal_uart_tx_byte(hal_uart_channel_t ch, uint8_t byte)
{
    (void)ch;
    (void)byte;
    /* TODO: Transmit byte via MCC driver */
}

void hal_uart_tx_string(hal_uart_channel_t ch, const char* str)
{
    while (*str) {
        hal_uart_tx_byte(ch, (uint8_t)*str++);
    }
}

bool hal_uart_rx_available(hal_uart_channel_t ch)
{
    (void)ch;
    /* TODO: Check RX available via MCC driver */
    return false;
}

bool hal_uart_rx_byte(hal_uart_channel_t ch, uint8_t* out_byte)
{
    (void)ch;
    (void)out_byte;
    /* TODO: Receive byte via MCC driver */
    return false;
}
