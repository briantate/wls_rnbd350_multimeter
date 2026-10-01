/**
 * @file led_svc.h
 * @brief Status LED control with state-dependent blink rate (M-05)
 */

#ifndef LED_SVC_H
#define LED_SVC_H

#include <stdint.h>

void led_svc_init(void);
void led_svc_update(uint32_t elapsed_ms);

#endif /* LED_SVC_H */
