/**
 * @file led_svc.c
 * @brief Status LED control with state-dependent blink rate (M-05)
 */

#include "led_svc.h"

#include <stdbool.h>
#include <stdint.h>

#include "../app/app_state.h"
#include "../hal/hal_gpio.h"

static uint32_t s_last_toggle_tick;

void led_svc_init(void)
{
    s_last_toggle_tick = 0;
    hal_gpio_write(HAL_PIN_LED, false);
}

void led_svc_update(uint32_t current_tick_ms)
{
    uint32_t threshold = app_state_is_streaming()
        ? LED_SVC_BLINK_PERIOD_STREAMING_MS
        : LED_SVC_BLINK_PERIOD_DISCONNECTED_MS;

    if ((current_tick_ms - s_last_toggle_tick) >= threshold) {
        hal_gpio_toggle(HAL_PIN_LED);
        s_last_toggle_tick = current_tick_ms;
    }
}
