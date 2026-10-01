/**
 * @file test_led_svc.cpp
 * @brief Unit tests for led_svc module (M-05)
 */

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"

extern "C" {
#include "services/led_svc.h"
#include "app/app_state.h"
#include "hal/hal_gpio.h"
}

TEST_GROUP(LedSvc)
{
    void setup() override
    {
        mock().clear();
        app_state_init();
        mock().disable();
        led_svc_init();
        mock().enable();
    }

    void teardown() override
    {
        mock().checkExpectations();
        mock().clear();
    }

    void setAppStateStreaming()
    {
        app_state_on_connect();
        app_state_on_stream_open();
    }

    void expectLedToggle()
    {
        mock().expectOneCall("hal_gpio_toggle")
              .withParameter("pin", (int)HAL_PIN_LED);
    }

    void expectNoLedToggle()
    {
        mock().expectNoCall("hal_gpio_toggle");
    }
};

/* TCI-047: led_svc_init turns LED off */
TEST(LedSvc, Init_TurnsLedOff)
{
    // SETUP
    mock().expectOneCall("hal_gpio_write")
          .withParameter("pin", (int)HAL_PIN_LED)
          .withParameter("state", false);

    // EXERCISE
    led_svc_init();

    // VERIFY: mock verifies LED written off (checked in teardown)

    // CLEANUP: handled by teardown
}

/* TCI-048: Toggles at 1000ms when disconnected */
TEST(LedSvc, Update_WhenDisconnected_TogglesAt1000ms)
{
    // SETUP: app_state is DISCONNECTED after setup()
    expectLedToggle();

    // EXERCISE: tick at 1000ms (1000ms since init at tick 0)
    led_svc_update(1000);

    // VERIFY: mock verifies toggle called (checked in teardown)

    // CLEANUP: handled by teardown
}

/* TCI-050: No toggle before 1000ms when disconnected */
TEST(LedSvc, Update_Before1000ms_NoToggle)
{
    // SETUP: app_state is DISCONNECTED after setup()
    expectNoLedToggle();

    // EXERCISE: tick at 999ms (999ms since init at tick 0)
    led_svc_update(999);

    // VERIFY: mock verifies no toggle (checked in teardown)

    // CLEANUP: handled by teardown
}

/* TCI-051: Toggles exactly at 1000ms (multiple update calls) */
TEST(LedSvc, Update_At1000ms_Toggles)
{
    // SETUP: app_state is DISCONNECTED after setup(); call at 500ms (no toggle)
    led_svc_update(500);
    expectLedToggle();

    // EXERCISE: call at 1000ms = 1000ms since init
    led_svc_update(1000);

    // VERIFY: mock verifies toggle at 1000ms boundary (checked in teardown)

    // CLEANUP: handled by teardown
}

/* TCI-056: Timer resets after toggle */
TEST(LedSvc, Update_ResetsAfterToggle)
{
    // SETUP: app_state is DISCONNECTED after setup(); trigger first toggle at 1000ms
    expectLedToggle();
    led_svc_update(1000);
    mock().checkExpectations();
    mock().clear();
    expectNoLedToggle();

    // EXERCISE: 999ms after last toggle (tick 1999) should not toggle
    led_svc_update(1999);

    // VERIFY: mock verifies no toggle (checked in teardown)

    // CLEANUP: handled by teardown
}

/* TCI-049: Toggles at 200ms when streaming */
TEST(LedSvc, Update_WhenStreaming_TogglesAt200ms)
{
    // SETUP
    setAppStateStreaming();
    expectLedToggle();

    // EXERCISE: tick at 200ms
    led_svc_update(200);

    // VERIFY: mock verifies toggle (checked in teardown)

    // CLEANUP: handled by teardown
}

/* TCI-052: No toggle before 200ms when streaming */
TEST(LedSvc, Update_Before200ms_NoToggle)
{
    // SETUP
    setAppStateStreaming();
    expectNoLedToggle();

    // EXERCISE: tick at 199ms
    led_svc_update(199);

    // VERIFY: mock verifies no toggle (checked in teardown)

    // CLEANUP: handled by teardown
}

/* TCI-053: Toggles exactly at 200ms when streaming (multiple calls) */
TEST(LedSvc, Update_At200ms_Toggles)
{
    // SETUP: streaming, call at 100ms (no toggle)
    setAppStateStreaming();
    led_svc_update(100);
    expectLedToggle();

    // EXERCISE: call at 200ms
    led_svc_update(200);

    // VERIFY: mock verifies toggle at 200ms (checked in teardown)

    // CLEANUP: handled by teardown
}

/* TCI-054: State change mid-cycle adjusts rate */
TEST(LedSvc, Update_StateChangesMidCycle_AdjustsRate)
{
    // SETUP: disconnected, call at 500ms (below 1000ms threshold, no toggle)
    led_svc_update(500);
    // Now switch to streaming — threshold becomes 200ms
    // Since 500ms already elapsed, next check should toggle
    setAppStateStreaming();
    expectLedToggle();

    // EXERCISE: call at 600ms (600ms since init, but threshold is now 200ms)
    led_svc_update(600);

    // VERIFY: mock verifies toggle (checked in teardown)

    // CLEANUP: handled by teardown
}

/* TCI-055: Multiple updates with no toggle until threshold */
TEST(LedSvc, Update_AccumulatesElapsedTime)
{
    // SETUP: disconnected; updates at 100, 200, ..., 900ms must not toggle
    mock().expectNoCall("hal_gpio_toggle");
    for (uint32_t tick = 100; tick <= 900; tick += 100) {
        led_svc_update(tick);
    }
    mock().checkExpectations();
    mock().clear();
    expectLedToggle();

    // EXERCISE: update at 1000ms triggers toggle
    led_svc_update(1000);

    // VERIFY: mock verifies toggle (checked in teardown)

    // CLEANUP: handled by teardown
}

/* TCI-057: Overflow-safe timing (new test) */
TEST(LedSvc, Update_OverflowSafe)
{
    // SETUP: Simulate near-overflow tick value
    // First, we need to set s_last_toggle_tick to near max by triggering a toggle
    uint32_t near_max = 0xFFFFFFFF - 500;  // 500ms before overflow

    // Trigger toggle at near_max (this is >= 1000ms from init's tick 0)
    expectLedToggle();
    led_svc_update(near_max);
    mock().checkExpectations();
    mock().clear();

    // Now s_last_toggle_tick = near_max
    // After overflow, tick wraps to small value
    // 501ms to overflow (0xFFFFFFFF - near_max + 1) + 499ms after = 1000ms total
    uint32_t after_overflow = 499;  // 500ms after wrap
    expectLedToggle();

    // EXERCISE: Should toggle because (after_overflow - near_max) wraps to ~1000ms
    // Math: 499 - (0xFFFFFFFF - 500) = 499 - 0xFFFFFE0B = 0x000003F4 = 1012 >= 1000
    led_svc_update(after_overflow);

    // VERIFY: mock verifies toggle even across overflow boundary

    // CLEANUP: handled by teardown
}
