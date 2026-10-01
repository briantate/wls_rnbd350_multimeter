/**
 * @file hal_tick_mock.cpp
 * @brief CppUTest mock for hal_tick (M-10)
 */

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"

extern "C" {
#include "hal/hal_tick.h"
}

static uint32_t mock_tick_ms = 0;

void hal_tick_init(void)
{
    mock_tick_ms = 0;
    mock().actualCall("hal_tick_init");
}

uint32_t hal_tick_get_ms(void)
{
    mock().actualCall("hal_tick_get_ms");
    return mock_tick_ms;
}

void hal_tick_mock_set_ms(uint32_t ms)
{
    mock_tick_ms = ms;
}

void hal_tick_mock_advance_ms(uint32_t ms)
{
    mock_tick_ms += ms;
}
