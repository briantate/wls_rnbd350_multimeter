/**
 * @file hal_tick.c
 * @brief System tick abstraction (1 ms timebase) (M-10)
 */

#include "hal_tick.h"

static volatile uint32_t tick_ms;

void hal_tick_init(void)
{
    tick_ms = 0;
    /* TODO: Initialize TC0 timer via MCC driver */
}

uint32_t hal_tick_get_ms(void)
{
    return tick_ms;
}

/* Called from TC0 ISR */
void hal_tick_isr(void)
{
    tick_ms++;
}
