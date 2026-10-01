/**
 * @file led_svc.h
 * @brief Status LED control with state-dependent blink rate (M-05)
 */

#ifndef BTOHM_SERVICES_LED_SVC_H_
#define BTOHM_SERVICES_LED_SVC_H_

#include <stdint.h>

#define LED_SVC_BLINK_PERIOD_DISCONNECTED_MS  (1000U)
#define LED_SVC_BLINK_PERIOD_STREAMING_MS     (200U)

/**
 * @brief Initialize the LED service.
 *
 * Turns LED off and resets blink timer.
 */
void led_svc_init(void);

/**
 * @brief Update LED blink state.
 *
 * Toggles LED based on elapsed time and connection state.
 *
 * @param elapsed_ms Milliseconds since last call.
 */
void led_svc_update(uint32_t elapsed_ms);

#endif  /* BTOHM_SERVICES_LED_SVC_H_ */
