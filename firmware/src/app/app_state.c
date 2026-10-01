/**
 * @file app_state.c
 * @brief Centralized application state machine (M-02)
 */

#include "app_state.h"

static app_state_t current_state;

void app_state_init(void)
{
    current_state = APP_STATE_DISCONNECTED;
}

app_state_t app_state_get(void)
{
    return current_state;
}

void app_state_on_connect(void)
{
    if (current_state == APP_STATE_DISCONNECTED) {
        current_state = APP_STATE_CONNECTED;
    }
}

void app_state_on_stream_open(void)
{
    if (current_state == APP_STATE_CONNECTED) {
        current_state = APP_STATE_STREAMING;
    }
}

void app_state_on_disconnect(void)
{
    current_state = APP_STATE_DISCONNECTED;
}

bool app_state_is_streaming(void)
{
    return current_state == APP_STATE_STREAMING;
}
