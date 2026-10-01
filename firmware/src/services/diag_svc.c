/**
 * @file diag_svc.c
 * @brief Diagnostic logging to dedicated UART (M-06)
 */

#include "diag_svc.h"
#include "../hal/hal_uart.h"
#include "../hal/hal_tick.h"
#include <stdarg.h>

void diag_svc_init(void)
{
    hal_uart_init(HAL_UART_DIAG);
}

void diag_svc_log(diag_level_t level, const char* subsys, const char* fmt, ...)
{
    (void)level;
    (void)subsys;
    (void)fmt;
    /* TODO: Format and transmit diagnostic message */
}
