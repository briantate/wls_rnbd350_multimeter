/**
 * @file ble_svc.c
 * @brief RNBD350 communication and JSON data transmission (M-04)
 */

#include "ble_svc.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "../hal/hal_uart.h"

static uint8_t rx_buffer[32];
static uint8_t rx_index = 0;

void ble_svc_init(void)
{
    hal_uart_init(HAL_UART_BLE);
    rx_index = 0;
    memset(rx_buffer, 0, sizeof(rx_buffer));
}

ble_event_t ble_svc_poll(void)
{
    uint8_t byte;

    while (hal_uart_rx_byte(HAL_UART_BLE, &byte)) {
        if (rx_index >= sizeof(rx_buffer) - 1) {
            rx_index = 0;
        }
        rx_buffer[rx_index++] = byte;
        rx_buffer[rx_index] = '\0';
    }

    char* start = strstr((char*)rx_buffer, "%CONNECT%");
    if (start != NULL) {
        uint8_t msg_start = start - (char*)rx_buffer;
        uint8_t msg_end = msg_start + 9;
        memmove(rx_buffer, rx_buffer + msg_end, rx_index - msg_end);
        rx_index -= msg_end;
        memset(rx_buffer + rx_index, 0, sizeof(rx_buffer) - rx_index);
        return BLE_EVENT_CONNECT;
    }

    start = strstr((char*)rx_buffer, "%DISCONNECT%");
    if (start != NULL) {
        uint8_t msg_start = start - (char*)rx_buffer;
        uint8_t msg_end = msg_start + 12;
        memmove(rx_buffer, rx_buffer + msg_end, rx_index - msg_end);
        rx_index -= msg_end;
        memset(rx_buffer + rx_index, 0, sizeof(rx_buffer) - rx_index);
        return BLE_EVENT_DISCONNECT;
    }

    start = strstr((char*)rx_buffer, "%STREAM_OPEN%");
    if (start != NULL) {
        uint8_t msg_start = start - (char*)rx_buffer;
        uint8_t msg_end = msg_start + 13;
        memmove(rx_buffer, rx_buffer + msg_end, rx_index - msg_end);
        rx_index -= msg_end;
        memset(rx_buffer + rx_index, 0, sizeof(rx_buffer) - rx_index);
        return BLE_EVENT_STREAM_OPEN;
    }

    return BLE_EVENT_NONE;
}

void ble_svc_transmit_measurement(uint32_t ohms)
{
    char json[64];
    snprintf(json, sizeof(json), "{\"Meter\":{\"ohm\":%lu}}\r\n", (unsigned long)ohms);
    hal_uart_tx_string(HAL_UART_BLE, json);
}

void ble_svc_discard_pending(void)
{
    rx_index = 0;
    memset(rx_buffer, 0, sizeof(rx_buffer));
}
