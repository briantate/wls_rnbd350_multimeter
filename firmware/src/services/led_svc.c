/**
 * @file led_svc.c
 * @brief Status LED control with state-dependent blink rate (M-05)
 */

#include "led_svc.h"
#include "../app/app_state.h"
#include "../hal/hal_gpio.h"

static uint32_t ms_since_toggle;

void led_svc_init(void)
{
    ms_since_toggle = 0;
    hal_gpio_write(HAL_PIN_LED, false);
}

void led_svc_update(uint32_t elapsed_ms)
{
    uint32_t threshold = app_state_is_streaming() ? 200 : 1000;

    ms_since_toggle += elapsed_ms;
    if (ms_since_toggle >= threshold) {
        hal_gpio_toggle(HAL_PIN_LED);
        ms_since_toggle = 0;
    }
}
