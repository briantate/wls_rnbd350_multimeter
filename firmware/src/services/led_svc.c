/**
 * @file led_svc.c
 * @brief Status LED control with state-dependent blink rate (M-05)
 */

#include "led_svc.h"

#include <stdbool.h>
#include <stdint.h>

#include "../app/app_state.h"
#include "../hal/hal_gpio.h"

static uint32_t s_ms_since_toggle;

void led_svc_init(void)
{
    s_ms_since_toggle = 0;
    hal_gpio_write(HAL_PIN_LED, false);
}

void led_svc_update(uint32_t elapsed_ms)
{
    uint32_t threshold = app_state_is_streaming()
        ? LED_SVC_BLINK_PERIOD_STREAMING_MS
        : LED_SVC_BLINK_PERIOD_DISCONNECTED_MS;

    s_ms_since_toggle += elapsed_ms;
    if (s_ms_since_toggle >= threshold) {
        hal_gpio_toggle(HAL_PIN_LED);
        s_ms_since_toggle = 0;
    }
}
