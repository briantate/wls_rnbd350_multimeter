/**
 * @file hal_uart_mock.cpp
 * @brief CppUTest mock for hal_uart (M-09)
 */

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"

extern "C" {
#include "hal/hal_uart.h"
}

void hal_uart_init(hal_uart_channel_t ch)
{
    mock().actualCall("hal_uart_init")
          .withParameter("ch", (int)ch);
}

bool hal_uart_tx_ready(hal_uart_channel_t ch)
{
    return mock().actualCall("hal_uart_tx_ready")
                 .withParameter("ch", (int)ch)
                 .returnBoolValueOrDefault(true);
}

void hal_uart_tx_byte(hal_uart_channel_t ch, uint8_t byte)
{
    mock().actualCall("hal_uart_tx_byte")
          .withParameter("ch", (int)ch)
          .withParameter("byte", byte);
}

void hal_uart_tx_string(hal_uart_channel_t ch, const char* str)
{
    mock().actualCall("hal_uart_tx_string")
          .withParameter("ch", (int)ch)
          .withParameter("str", str);
}

bool hal_uart_rx_available(hal_uart_channel_t ch)
{
    return mock().actualCall("hal_uart_rx_available")
                 .withParameter("ch", (int)ch)
                 .returnBoolValueOrDefault(false);
}

bool hal_uart_rx_byte(hal_uart_channel_t ch, uint8_t* byte)
{
    return mock().actualCall("hal_uart_rx_byte")
                 .withParameter("ch", (int)ch)
                 .withOutputParameter("byte", byte)
                 .returnBoolValueOrDefault(false);
}
