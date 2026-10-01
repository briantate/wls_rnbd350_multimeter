/**
 * @file app_state.h
 * @brief Centralized application state machine (M-02)
 */

#ifndef APP_STATE_H
#define APP_STATE_H

#include <stdbool.h>

typedef enum {
    APP_STATE_DISCONNECTED,
    APP_STATE_CONNECTED,
    APP_STATE_STREAMING
} app_state_t;

void        app_state_init(void);
app_state_t app_state_get(void);
void        app_state_on_connect(void);
void        app_state_on_stream_open(void);
void        app_state_on_disconnect(void);
bool        app_state_is_streaming(void);

#endif /* APP_STATE_H */
