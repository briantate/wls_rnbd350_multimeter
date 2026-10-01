/**
 * @file test-declarations.cpp
 * @brief Test declarations for hal_tick module (M-10)
 * @date 2026-09-30
 */

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"

extern "C" {
#include "hal/hal_tick.h"
}

TEST_GROUP(HalTick)
{
    void setup() override
    {
        mock().clear();
        hal_tick_init();
    }

    void teardown() override
    {
        mock().checkExpectations();
        mock().clear();
    }
};

/* TCI-096: hal_tick_init starts timer */
TEST(HalTick, Init_StartsTimer)
{
    // Arrange
    mock().expectOneCall("TC0_TimerInitialize");
    mock().expectOneCall("TC0_TimerStart");

    // Act
    hal_tick_init();

    // Assert: timer configured and started
}

/* TCI-097: get_ms returns milliseconds */
TEST(HalTick, GetMs_ReturnsMilliseconds)
{
    // Arrange: Mock returns specific value
    mock().expectOneCall("hal_tick_mock_get_ms")
          .andReturnValue(1000);

    // Act
    uint32_t ms = hal_tick_get_ms();

    // Assert
    CHECK_EQUAL(1000, ms);
}

/* TCI-098: get_ms increments over time */
TEST(HalTick, GetMs_IncrementsOverTime)
{
    // Arrange
    mock().expectOneCall("hal_tick_mock_get_ms").andReturnValue(100);
    uint32_t t1 = hal_tick_get_ms();

    mock().expectOneCall("hal_tick_mock_get_ms").andReturnValue(150);

    // Act
    uint32_t t2 = hal_tick_get_ms();

    // Assert
    CHECK_TRUE(t2 > t1);
    CHECK_EQUAL(50, t2 - t1);
}

/* TCI-099: Wraparound arithmetic works correctly */
TEST(HalTick, GetMs_WraparoundSafe)
{
    // Arrange: Time near max uint32
    mock().expectOneCall("hal_tick_mock_get_ms").andReturnValue(0xFFFFFFF0);
    uint32_t t1 = hal_tick_get_ms();

    // Time wrapped around
    mock().expectOneCall("hal_tick_mock_get_ms").andReturnValue(0x00000010);

    // Act
    uint32_t t2 = hal_tick_get_ms();

    // Assert: Unsigned subtraction handles wraparound
    uint32_t elapsed = t2 - t1;
    CHECK_EQUAL(32, elapsed);  // 0x10 - 0xFFFFFFF0 = 0x20 = 32
}
