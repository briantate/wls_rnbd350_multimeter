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

/* TCI-099: hal_tick_init starts timer */
TEST(HalTick, Init_StartsTimer)
{
    // Arrange
    mock().expectOneCall("TC0_TimerInitialize");
    mock().expectOneCall("TC0_TimerStart");

    // Act
    hal_tick_init();

    // Assert: timer configured and started
}

/* TCI-100: get_ms returns milliseconds */
TEST(HalTick, GetMs_ReturnsMilliseconds)
{
    // Arrange: Advance time in mock
    hal_tick_advance_ms(1000);

    // Act
    uint32_t ms = hal_tick_get_ms();

    // Assert
    CHECK_EQUAL(1000, ms);
}

/* TCI-101: get_ms increments over time */
TEST(HalTick, GetMs_IncrementsOverTime)
{
    // Arrange
    uint32_t t1 = hal_tick_get_ms();
    hal_tick_advance_ms(50);

    // Act
    uint32_t t2 = hal_tick_get_ms();

    // Assert
    CHECK_EQUAL(t1 + 50, t2);
}

/* TCI-102: check_flag returns true after tick */
TEST(HalTick, CheckFlag_AfterTick_ReturnsTrue)
{
    // Arrange: Simulate tick interrupt setting flag
    hal_tick_advance_ms(1);  // Mock sets flag on advance

    // Act
    bool flag = hal_tick_check_flag();

    // Assert
    CHECK_TRUE(flag);
}

/* TCI-103: check_flag clears the flag */
TEST(HalTick, CheckFlag_ClearsFlag)
{
    // Arrange
    hal_tick_advance_ms(1);
    CHECK_TRUE(hal_tick_check_flag());

    // Act: Second check without new tick
    bool flag = hal_tick_check_flag();

    // Assert: Flag was cleared by first check
    CHECK_FALSE(flag);
}

/* TCI-104: check_flag returns false before tick */
TEST(HalTick, CheckFlag_NoTick_ReturnsFalse)
{
    // Arrange: Fresh init, no ticks yet
    hal_tick_init();

    // Act
    bool flag = hal_tick_check_flag();

    // Assert
    CHECK_FALSE(flag);
}

/* TCI-105: clear_flag clears the flag */
TEST(HalTick, ClearFlag_ClearsFlag)
{
    // Arrange
    hal_tick_advance_ms(1);
    CHECK_TRUE(hal_tick_check_flag());

    // Arrange: Another tick
    hal_tick_advance_ms(1);

    // Act: Clear without checking
    hal_tick_clear_flag();

    // Assert
    CHECK_FALSE(hal_tick_check_flag());
}

/* TCI-106: advance_ms updates time (mock test) */
TEST(HalTick, AdvanceMs_Mock_AdvancesTime)
{
    // Arrange
    uint32_t initial = hal_tick_get_ms();

    // Act
    hal_tick_advance_ms(12345);

    // Assert
    CHECK_EQUAL(initial + 12345, hal_tick_get_ms());
}
