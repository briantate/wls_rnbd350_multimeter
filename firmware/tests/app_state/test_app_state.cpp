/**
 * @file test_app_state.cpp
 * @brief Unit tests for app_state module (M-02)
 */

#include "CppUTest/TestHarness.h"

extern "C" {
#include "app/app_state.h"
}

TEST_GROUP(AppState)
{
    void setup()
    {
        app_state_init();
    }

    void teardown()
    {
    }
};

/* TCI-001: app_state_init sets state to DISCONNECTED */
TEST(AppState, Init_SetsDisconnected)
{
    // SETUP
    // (setup() calls app_state_init())

    // EXERCISE
    // (init already called in setup)

    // VERIFY
    CHECK_EQUAL(APP_STATE_DISCONNECTED, app_state_get());

    // CLEANUP
    // None needed
}

/* TCI-002: app_state_get returns current state after changes */
TEST(AppState, Get_ReturnsCurrentState)
{
    // SETUP
    // State starts as DISCONNECTED from setup()
    app_state_on_connect();

    // EXERCISE
    app_state_t state = app_state_get();

    // VERIFY
    CHECK_EQUAL(APP_STATE_CONNECTED, state);

    // CLEANUP
    // None needed
}

/* TCI-003: app_state_on_connect when DISCONNECTED transitions to CONNECTED */
TEST(AppState, OnConnect_WhenDisconnected_TransitionsToConnected)
{
    // SETUP
    // (setup() initializes state to DISCONNECTED)

    // EXERCISE
    app_state_on_connect();

    // VERIFY
    CHECK_EQUAL(APP_STATE_CONNECTED, app_state_get());

    // CLEANUP
    // None needed
}

/* TCI-004: app_state_on_connect when already CONNECTED remains CONNECTED */
TEST(AppState, OnConnect_WhenConnected_RemainsConnected)
{
    // SETUP
    app_state_on_connect();
    CHECK_EQUAL(APP_STATE_CONNECTED, app_state_get());

    // EXERCISE
    app_state_on_connect();

    // VERIFY
    CHECK_EQUAL(APP_STATE_CONNECTED, app_state_get());

    // CLEANUP
    // None needed
}

/* TCI-005: app_state_on_connect when STREAMING remains STREAMING */
TEST(AppState, OnConnect_WhenStreaming_RemainsStreaming)
{
    // SETUP
    app_state_on_connect();
    app_state_on_stream_open();
    CHECK_EQUAL(APP_STATE_STREAMING, app_state_get());

    // EXERCISE
    app_state_on_connect();

    // VERIFY
    CHECK_EQUAL(APP_STATE_STREAMING, app_state_get());

    // CLEANUP
    // None needed
}

/* TCI-006: app_state_on_stream_open when CONNECTED transitions to STREAMING */
TEST(AppState, OnStreamOpen_WhenConnected_TransitionsToStreaming)
{
    // SETUP
    app_state_on_connect();
    CHECK_EQUAL(APP_STATE_CONNECTED, app_state_get());

    // EXERCISE
    app_state_on_stream_open();

    // VERIFY
    CHECK_EQUAL(APP_STATE_STREAMING, app_state_get());

    // CLEANUP
    // None needed
}

/* TCI-007: app_state_on_stream_open when DISCONNECTED remains DISCONNECTED */
TEST(AppState, OnStreamOpen_WhenDisconnected_RemainsDisconnected)
{
    // SETUP
    // (setup() initializes state to DISCONNECTED)

    // EXERCISE
    app_state_on_stream_open();

    // VERIFY
    CHECK_EQUAL(APP_STATE_DISCONNECTED, app_state_get());

    // CLEANUP
    // None needed
}

/* TCI-008: app_state_on_stream_open when already STREAMING remains STREAMING */
TEST(AppState, OnStreamOpen_WhenStreaming_RemainsStreaming)
{
    // SETUP
    app_state_on_connect();
    app_state_on_stream_open();
    CHECK_EQUAL(APP_STATE_STREAMING, app_state_get());

    // EXERCISE
    app_state_on_stream_open();

    // VERIFY
    CHECK_EQUAL(APP_STATE_STREAMING, app_state_get());

    // CLEANUP
    // None needed
}

/* TCI-009: app_state_on_disconnect when CONNECTED transitions to DISCONNECTED */
TEST(AppState, OnDisconnect_WhenConnected_TransitionsToDisconnected)
{
    // SETUP
    app_state_on_connect();

    // EXERCISE
    app_state_on_disconnect();

    // VERIFY
    CHECK_EQUAL(APP_STATE_DISCONNECTED, app_state_get());

    // CLEANUP
    // None needed
}

/* TCI-010: app_state_on_disconnect when STREAMING transitions to DISCONNECTED */
TEST(AppState, OnDisconnect_WhenStreaming_TransitionsToDisconnected)
{
    // SETUP
    app_state_on_connect();
    app_state_on_stream_open();

    // EXERCISE
    app_state_on_disconnect();

    // VERIFY
    CHECK_EQUAL(APP_STATE_DISCONNECTED, app_state_get());

    // CLEANUP
    // None needed
}

/* TCI-011: app_state_is_streaming when STREAMING returns true */
TEST(AppState, IsStreaming_WhenStreaming_ReturnsTrue)
{
    // SETUP
    app_state_on_connect();
    app_state_on_stream_open();

    // EXERCISE
    bool result = app_state_is_streaming();

    // VERIFY
    CHECK_TRUE(result);

    // CLEANUP
    // None needed
}

/* TCI-012: app_state_is_streaming when not STREAMING returns false */
TEST(AppState, IsStreaming_WhenNotStreaming_ReturnsFalse)
{
    // SETUP & VERIFY: DISCONNECTED state
    CHECK_FALSE(app_state_is_streaming());

    // SETUP: CONNECTED state
    app_state_on_connect();

    // EXERCISE & VERIFY
    CHECK_FALSE(app_state_is_streaming());

    // CLEANUP
    // None needed
}
