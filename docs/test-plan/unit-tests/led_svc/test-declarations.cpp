/**
 * @file test-declarations.cpp
 * @brief Test declarations for led_svc module (M-05)
 * @date 2026-09-30
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
        led_svc_init();
    }

    void teardown() override
    {
        mock().checkExpectations();
        mock().clear();
    }

    void mockAppStateDisconnected()
    {
        mock().expectOneCall("app_state_is_streaming")
              .andReturnValue(false);
        mock().expectOneCall("app_state_get")
              .andReturnValue((int)APP_STATE_DISCONNECTED);
    }

    void mockAppStateStreaming()
    {
        mock().expectOneCall("app_state_is_streaming")
              .andReturnValue(true);
    }

    void expectLedToggle()
    {
        mock().expectOneCall("hal_gpio_toggle")
              .withParameter("pin", HAL_PIN_LED);
    }

    void expectNoLedToggle()
    {
        mock().expectNoCall("hal_gpio_toggle");
    }
};

/* TCI-047: led_svc_init turns LED off */
TEST(LedSvc, Init_TurnsLedOff)
{
    // Arrange
    mock().expectOneCall("hal_gpio_write")
          .withParameter("pin", HAL_PIN_LED)
          .withParameter("state", false);

    // Act
    led_svc_init();

    // Assert: mock verifies LED set to off
}

/* TCI-048: Toggles at 1000ms when disconnected */
TEST(LedSvc, Update_WhenDisconnected_TogglesAt1000ms)
{
    // Arrange
    mockAppStateDisconnected();
    expectLedToggle();

    // Act: Accumulate 1000ms
    led_svc_update(1000);

    // Assert: mock verifies toggle called
}

/* TCI-049: Toggles at 200ms when streaming */
TEST(LedSvc, Update_WhenStreaming_TogglesAt200ms)
{
    // Arrange
    mockAppStateStreaming();
    expectLedToggle();

    // Act
    led_svc_update(200);

    // Assert: mock verifies toggle
}

/* TCI-050: No toggle before 1000ms when disconnected */
TEST(LedSvc, Update_Before1000ms_NoToggle)
{
    // Arrange
    mockAppStateDisconnected();
    expectNoLedToggle();

    // Act
    led_svc_update(999);

    // Assert: no toggle called
}

/* TCI-051: Toggles exactly at 1000ms */
TEST(LedSvc, Update_At1000ms_Toggles)
{
    // Arrange: First call with 500ms
    mock().expectOneCall("app_state_is_streaming").andReturnValue(false);
    mock().expectOneCall("app_state_get").andReturnValue((int)APP_STATE_DISCONNECTED);
    led_svc_update(500);

    // Arrange: Second call with another 500ms = 1000ms total
    mock().expectOneCall("app_state_is_streaming").andReturnValue(false);
    mock().expectOneCall("app_state_get").andReturnValue((int)APP_STATE_DISCONNECTED);
    expectLedToggle();

    // Act
    led_svc_update(500);

    // Assert: toggle happens at 1000ms boundary
}

/* TCI-052: No toggle before 200ms when streaming */
TEST(LedSvc, Update_Before200ms_NoToggle)
{
    // Arrange
    mockAppStateStreaming();
    expectNoLedToggle();

    // Act
    led_svc_update(199);

    // Assert: no toggle
}

/* TCI-053: Toggles exactly at 200ms when streaming */
TEST(LedSvc, Update_At200ms_Toggles)
{
    // Arrange: First call with 100ms
    mock().expectOneCall("app_state_is_streaming").andReturnValue(true);
    led_svc_update(100);

    // Arrange: Second call with another 100ms = 200ms total
    mock().expectOneCall("app_state_is_streaming").andReturnValue(true);
    expectLedToggle();

    // Act
    led_svc_update(100);

    // Assert: toggle at 200ms
}

/* TCI-054: State change mid-cycle adjusts rate */
TEST(LedSvc, Update_StateChangesMidCycle_AdjustsRate)
{
    // Arrange: Start disconnected, accumulate 500ms
    mock().expectOneCall("app_state_is_streaming").andReturnValue(false);
    mock().expectOneCall("app_state_get").andReturnValue((int)APP_STATE_DISCONNECTED);
    led_svc_update(500);

    // Arrange: State changes to streaming at 600ms total
    // But we've accumulated 500ms, and streaming threshold is 200ms
    // Should toggle immediately if accumulated >= 200ms
    mock().expectOneCall("app_state_is_streaming").andReturnValue(true);
    expectLedToggle();

    // Act
    led_svc_update(100);  // 600ms total, but streaming rate applies

    // Assert: toggle happens because 600ms > 200ms threshold
}

/* TCI-055: Accumulates elapsed time across calls */
TEST(LedSvc, Update_AccumulatesElapsedTime)
{
    // Arrange & Act: Multiple small updates
    for (int i = 0; i < 9; i++) {
        mock().expectOneCall("app_state_is_streaming").andReturnValue(false);
        mock().expectOneCall("app_state_get").andReturnValue((int)APP_STATE_DISCONNECTED);
        led_svc_update(100);  // 100ms each, no toggle yet
    }

    // Arrange: 10th update reaches 1000ms
    mock().expectOneCall("app_state_is_streaming").andReturnValue(false);
    mock().expectOneCall("app_state_get").andReturnValue((int)APP_STATE_DISCONNECTED);
    expectLedToggle();

    // Act
    led_svc_update(100);  // 1000ms total

    // Assert: toggle happens
}

/* TCI-056: Timer resets after toggle */
TEST(LedSvc, Update_ResetsAfterToggle)
{
    // Arrange: Trigger first toggle
    mockAppStateDisconnected();
    expectLedToggle();
    led_svc_update(1000);

    mock().checkExpectations();
    mock().clear();

    // Arrange: Another 999ms should NOT trigger toggle
    mockAppStateDisconnected();
    expectNoLedToggle();

    // Act
    led_svc_update(999);

    // Assert: no toggle (timer reset after first)
}
