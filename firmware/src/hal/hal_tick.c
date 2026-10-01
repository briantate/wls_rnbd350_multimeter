/**
 * @file hal_tick.c
 * @brief System tick abstraction (1 ms timebase) (M-10)
 */

#include "hal_tick.h"

#include <stdint.h>

static volatile uint32_t s_tick_ms;

void hal_tick_init(void)
{
    s_tick_ms = 0;
    /* TODO: Initialize TC0 timer via MCC driver */
}

uint32_t hal_tick_get_ms(void)
{
    return s_tick_ms;
}

void hal_tick_isr(void)
{
    s_tick_ms++;
}
