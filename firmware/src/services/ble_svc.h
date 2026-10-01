/**
 * @file ble_svc.h
 * @brief RNBD350 communication and JSON data transmission (M-04)
 */

#ifndef BLE_SVC_H
#define BLE_SVC_H

#include <stdint.h>

typedef enum {
    BLE_EVENT_NONE,
    BLE_EVENT_CONNECT,
    BLE_EVENT_DISCONNECT,
    BLE_EVENT_STREAM_OPEN
} ble_event_t;

void        ble_svc_init(void);
ble_event_t ble_svc_poll(void);
void        ble_svc_transmit_measurement(uint32_t ohms);
void        ble_svc_discard_pending(void);

#endif /* BLE_SVC_H */
