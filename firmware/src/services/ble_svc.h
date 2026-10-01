/**
 * @file ble_svc.h
 * @brief RNBD350 communication and JSON data transmission (M-04)
 */

#ifndef BTOHM_SERVICES_BLE_SVC_H_
#define BTOHM_SERVICES_BLE_SVC_H_

#include <stdint.h>

typedef enum {
    BLE_EVENT_NONE,
    BLE_EVENT_CONNECT,
    BLE_EVENT_DISCONNECT,
    BLE_EVENT_STREAM_OPEN
} ble_event_t;

/**
 * @brief Initialize the BLE service.
 *
 * Configures UART for RNBD350 communication.
 */
void ble_svc_init(void);

/**
 * @brief Poll for BLE events.
 *
 * Parses incoming UART data for status messages.
 *
 * @return Event type, or BLE_EVENT_NONE if no event.
 */
ble_event_t ble_svc_poll(void);

/**
 * @brief Transmit a measurement over BLE.
 *
 * Formats resistance as JSON and sends via UART.
 *
 * @param ohms Resistance value in ohms.
 */
void ble_svc_transmit_measurement(uint32_t ohms);

/**
 * @brief Discard pending TX and RX data.
 *
 * Called on disconnect to clear buffers.
 */
void ble_svc_discard_pending(void);

#endif  /* BTOHM_SERVICES_BLE_SVC_H_ */
