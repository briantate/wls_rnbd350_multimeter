/**
 * @file hal_tick.h
 * @brief System tick abstraction (1 ms timebase) (M-10)
 */

#ifndef BTOHM_HAL_HAL_TICK_H_
#define BTOHM_HAL_HAL_TICK_H_

#include <stdint.h>

/**
 * @brief Initialize the system tick timer.
 */
void hal_tick_init(void);

/**
 * @brief Get milliseconds since boot.
 *
 * @return Millisecond count (wraps at ~49 days).
 */
uint32_t hal_tick_get_ms(void);

#endif  /* BTOHM_HAL_HAL_TICK_H_ */
