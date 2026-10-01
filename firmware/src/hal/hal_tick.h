/**
 * @file hal_tick.h
 * @brief System tick abstraction (1 ms timebase) (M-10)
 */

#ifndef HAL_TICK_H
#define HAL_TICK_H

#include <stdint.h>

void     hal_tick_init(void);
uint32_t hal_tick_get_ms(void);

#endif /* HAL_TICK_H */
