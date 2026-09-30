/**
 * @file test-declarations.cpp
 * @brief Test declarations for app_state module (M-02)
 * @date 2026-09-30
 *
 * These are compilable test stubs following the naming convention:
 * <ModuleName>_<FunctionUnderTest>_<Scenario>_<ExpectedResult>
 */

#include "CppUTest/TestHarness.h"

extern "C" {
#include "app/app_state.h"
}

TEST_GROUP(AppState)
{
    void setup() override
    {
        app_state_init();
    }

    void teardown() override
    {
    }
};

/* TCI-001: app_state_init */
TEST(AppState, Init_SetsDisconnected)
{
    // Arrange: (setup calls init)

    // Act: (init called in setup)

    // Assert
    CHECK_EQUAL(APP_STATE_DISCONNECTED, app_state_get());
}

/* TCI-002: app_state_get */
TEST(AppState, Get_ReturnsCurrentState)
{
    // Arrange
    app_state_on_connect();

    // Act
    app_state_t state = app_state_get();

    // Assert
    CHECK_EQUAL(APP_STATE_CONNECTED, state);
}

/* TCI-003: app_state_on_connect when disconnected */
TEST(AppState, OnConnect_WhenDisconnected_TransitionsToConnected)
{
    // Arrange: init sets to DISCONNECTED

    // Act
    app_state_on_connect();

    // Assert
    CHECK_EQUAL(APP_STATE_CONNECTED, app_state_get());
}

/* TCI-004: app_state_on_connect when already connected */
TEST(AppState, OnConnect_WhenConnected_RemainsConnected)
{
    // Arrange
    app_state_on_connect();
    CHECK_EQUAL(APP_STATE_CONNECTED, app_state_get());

    // Act
    app_state_on_connect();

    // Assert
    CHECK_EQUAL(APP_STATE_CONNECTED, app_state_get());
}

/* TCI-005: app_state_on_connect when streaming */
TEST(AppState, OnConnect_WhenStreaming_RemainsStreaming)
{
    // Arrange
    app_state_on_connect();
    app_state_on_stream_open();
    CHECK_EQUAL(APP_STATE_STREAMING, app_state_get());

    // Act
    app_state_on_connect();

    // Assert
    CHECK_EQUAL(APP_STATE_STREAMING, app_state_get());
}

/* TCI-006: app_state_on_stream_open when connected */
TEST(AppState, OnStreamOpen_WhenConnected_TransitionsToStreaming)
{
    // Arrange
    app_state_on_connect();

    // Act
    app_state_on_stream_open();

    // Assert
    CHECK_EQUAL(APP_STATE_STREAMING, app_state_get());
}

/* TCI-007: app_state_on_stream_open when disconnected (invalid) */
TEST(AppState, OnStreamOpen_WhenDisconnected_RemainsDisconnected)
{
    // Arrange: init sets to DISCONNECTED

    // Act
    app_state_on_stream_open();

    // Assert: Should not transition without connection
    CHECK_EQUAL(APP_STATE_DISCONNECTED, app_state_get());
}

/* TCI-008: app_state_on_stream_open when already streaming */
TEST(AppState, OnStreamOpen_WhenStreaming_RemainsStreaming)
{
    // Arrange
    app_state_on_connect();
    app_state_on_stream_open();
    CHECK_EQUAL(APP_STATE_STREAMING, app_state_get());

    // Act
    app_state_on_stream_open();

    // Assert
    CHECK_EQUAL(APP_STATE_STREAMING, app_state_get());
}

/* TCI-009: app_state_on_disconnect when connected */
TEST(AppState, OnDisconnect_WhenConnected_TransitionsToDisconnected)
{
    // Arrange
    app_state_on_connect();

    // Act
    app_state_on_disconnect();

    // Assert
    CHECK_EQUAL(APP_STATE_DISCONNECTED, app_state_get());
}

/* TCI-010: app_state_on_disconnect when streaming */
TEST(AppState, OnDisconnect_WhenStreaming_TransitionsToDisconnected)
{
    // Arrange
    app_state_on_connect();
    app_state_on_stream_open();

    // Act
    app_state_on_disconnect();

    // Assert
    CHECK_EQUAL(APP_STATE_DISCONNECTED, app_state_get());
}

/* TCI-011: app_state_is_streaming when streaming */
TEST(AppState, IsStreaming_WhenStreaming_ReturnsTrue)
{
    // Arrange
    app_state_on_connect();
    app_state_on_stream_open();

    // Act
    bool result = app_state_is_streaming();

    // Assert
    CHECK_TRUE(result);
}

/* TCI-012: app_state_is_streaming when not streaming */
TEST(AppState, IsStreaming_WhenNotStreaming_ReturnsFalse)
{
    // Arrange: DISCONNECTED state
    CHECK_FALSE(app_state_is_streaming());

    // Arrange: CONNECTED state
    app_state_on_connect();

    // Act & Assert
    CHECK_FALSE(app_state_is_streaming());
}
