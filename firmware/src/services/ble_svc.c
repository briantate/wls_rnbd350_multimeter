/**
 * @file ble_svc.c
 * @brief RNBD350 communication and JSON data transmission (M-04)
 */

#include "ble_svc.h"
#include "../hal/hal_uart.h"

void ble_svc_init(void)
{
    hal_uart_init(HAL_UART_BLE);
}

ble_event_t ble_svc_poll(void)
{
    /* TODO: Parse incoming status messages */
    return BLE_EVENT_NONE;
}

void ble_svc_transmit_measurement(uint32_t ohms)
{
    (void)ohms;
    /* TODO: Format and transmit JSON */
}

void ble_svc_discard_pending(void)
{
    /* TODO: Clear TX and RX buffers */
}
