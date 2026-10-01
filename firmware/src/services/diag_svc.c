/**
 * @file diag_svc.c
 * @brief Diagnostic logging to dedicated UART (M-06)
 */

#include "diag_svc.h"

#include <stdarg.h>

#include "../hal/hal_uart.h"
#include "../hal/hal_tick.h"

void diag_svc_init(void)
{
    hal_uart_init(HAL_UART_DIAG);
}

void diag_svc_log(diag_level_t level, const char* subsys, const char* fmt, ...)
{
    (void)level;
    (void)subsys;
    va_list args;
    va_start(args, fmt);
    (void)args;
    /* TODO: Format and transmit diagnostic message */
    va_end(args);
}
