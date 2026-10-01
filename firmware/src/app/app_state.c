/**
 * @file app_state.c
 * @brief Centralized application state machine (M-02)
 */

#include "app_state.h"

#include <stdbool.h>

static app_state_t s_current_state;

void app_state_init(void)
{
    s_current_state = APP_STATE_DISCONNECTED;
}

app_state_t app_state_get(void)
{
    return s_current_state;
}

void app_state_on_connect(void)
{
    if (s_current_state != APP_STATE_STREAMING) {
        s_current_state = APP_STATE_CONNECTED;
    }
}

void app_state_on_stream_open(void)
{
    if (s_current_state == APP_STATE_CONNECTED) {
        s_current_state = APP_STATE_STREAMING;
    }
}

void app_state_on_disconnect(void)
{
    s_current_state = APP_STATE_DISCONNECTED;
}

bool app_state_is_streaming(void)
{
    return (s_current_state == APP_STATE_STREAMING);
}
