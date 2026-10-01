/**
 * @file measurement_svc.h
 * @brief Resistance measurement acquisition and conversion (M-03)
 */

#ifndef BTOHM_SERVICES_MEASUREMENT_SVC_H_
#define BTOHM_SERVICES_MEASUREMENT_SVC_H_

#include <stdbool.h>
#include <stdint.h>

typedef struct {
    uint32_t ohms;
    bool     valid;
} measurement_t;

/**
 * @brief Initialize the measurement service.
 *
 * Configures range selection GPIOs.
 */
void measurement_svc_init(void);

/**
 * @brief Take a resistance measurement sample.
 *
 * Reads the ADC and converts to ohms.
 *
 * @return Measurement result with validity flag.
 */
measurement_t measurement_svc_sample(void);

#endif  /* BTOHM_SERVICES_MEASUREMENT_SVC_H_ */
