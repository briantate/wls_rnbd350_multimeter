/**
 * @file hal_gpio_mock.cpp
 * @brief CppUTest mock for hal_gpio (M-07)
 */

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"

extern "C" {
#include "hal/hal_gpio.h"
}

void hal_gpio_init(void)
{
    mock().actualCall("hal_gpio_init");
}

void hal_gpio_write(hal_pin_t pin, bool state)
{
    mock().actualCall("hal_gpio_write")
          .withParameter("pin", (int)pin)
          .withParameter("state", state);
}

bool hal_gpio_read(hal_pin_t pin)
{
    return mock().actualCall("hal_gpio_read")
                 .withParameter("pin", (int)pin)
                 .returnBoolValueOrDefault(false);
}

void hal_gpio_toggle(hal_pin_t pin)
{
    mock().actualCall("hal_gpio_toggle")
          .withParameter("pin", (int)pin);
}
