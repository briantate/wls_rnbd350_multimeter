/**
 * @file main.c
 * @brief Entry point and super-loop orchestration (M-01)
 */

#include "app_state.h"
#include "../services/measurement_svc.h"
#include "../services/ble_svc.h"
#include "../services/led_svc.h"
#include "../services/diag_svc.h"
#include "../hal/hal_tick.h"
#include "../hal/hal_gpio.h"

#define SAMPLE_INTERVAL_MS 100

int main(void)
{
    hal_gpio_init();
    hal_tick_init();

    app_state_init();
    measurement_svc_init();
    ble_svc_init();
    led_svc_init();
    diag_svc_init();

    uint32_t last_sample_ms = 0;
    uint32_t last_tick_ms = hal_tick_get_ms();

    while (1) {
        uint32_t now_ms = hal_tick_get_ms();
        uint32_t elapsed_ms = now_ms - last_tick_ms;
        last_tick_ms = now_ms;

        ble_event_t event = ble_svc_poll();
        switch (event) {
            case BLE_EVENT_CONNECT:
                app_state_on_connect();
                break;
            case BLE_EVENT_STREAM_OPEN:
                app_state_on_stream_open();
                break;
            case BLE_EVENT_DISCONNECT:
                app_state_on_disconnect();
                ble_svc_discard_pending();
                break;
            case BLE_EVENT_NONE:
            default:
                break;
        }

        if (app_state_is_streaming()) {
            if ((now_ms - last_sample_ms) >= SAMPLE_INTERVAL_MS) {
                last_sample_ms = now_ms;
                measurement_t m = measurement_svc_sample();
                if (m.valid) {
                    ble_svc_transmit_measurement(m.ohms);
                }
            }
        }

        led_svc_update(elapsed_ms);
    }

    return 0;
}
