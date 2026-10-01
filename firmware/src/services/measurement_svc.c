/**
 * @file measurement_svc.c
 * @brief Resistance measurement acquisition and conversion (M-03)
 */

#include "measurement_svc.h"

#include <stdbool.h>
#include <stdint.h>

#include "../hal/hal_gpio.h"
#include "../hal/hal_spi.h"

void measurement_svc_init(void)
{
    /* TODO: Configure range selection GPIOs */
}

measurement_t measurement_svc_sample(void)
{
    measurement_t result = {0, false};
    /* TODO: Read ADC and convert to ohms */
    return result;
}
