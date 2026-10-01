/**
 * @file app_state.h
 * @brief Centralized application state machine (M-02)
 */

#ifndef BTOHM_APP_APP_STATE_H_
#define BTOHM_APP_APP_STATE_H_

#include <stdbool.h>

typedef enum {
    APP_STATE_DISCONNECTED,
    APP_STATE_CONNECTED,
    APP_STATE_STREAMING
} app_state_t;

/**
 * @brief Initialize the application state machine.
 *
 * Sets state to APP_STATE_DISCONNECTED.
 */
void app_state_init(void);

/**
 * @brief Get the current application state.
 *
 * @return Current state value.
 */
app_state_t app_state_get(void);

/**
 * @brief Handle BLE connect event.
 *
 * Transitions from DISCONNECTED to CONNECTED.
 */
void app_state_on_connect(void);

/**
 * @brief Handle BLE stream open event.
 *
 * Transitions from CONNECTED to STREAMING.
 */
void app_state_on_stream_open(void);

/**
 * @brief Handle BLE disconnect event.
 *
 * Transitions to DISCONNECTED from any state.
 */
void app_state_on_disconnect(void);

/**
 * @brief Check if currently in streaming state.
 *
 * @return true if streaming, false otherwise.
 */
bool app_state_is_streaming(void);

#endif  /* BTOHM_APP_APP_STATE_H_ */
