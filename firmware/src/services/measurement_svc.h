/**
 * @file measurement_svc.h
 * @brief Resistance measurement acquisition and conversion (M-03)
 */

#ifndef MEASUREMENT_SVC_H
#define MEASUREMENT_SVC_H

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    uint32_t ohms;
    bool     valid;
} measurement_t;

void          measurement_svc_init(void);
measurement_t measurement_svc_sample(void);

#endif /* MEASUREMENT_SVC_H */
