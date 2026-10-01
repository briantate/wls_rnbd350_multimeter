/**
 * @file hal_spi_mock.cpp
 * @brief CppUTest mock for hal_spi (M-08)
 */

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"

extern "C" {
#include "hal/hal_spi.h"
}

void hal_spi_init(void)
{
    mock().actualCall("hal_spi_init");
}

void hal_spi_transfer(const uint8_t* tx, uint8_t* rx, size_t len)
{
    mock().actualCall("hal_spi_transfer")
          .withMemoryBufferParameter("tx", tx, len)
          .withOutputParameter("rx", rx)
          .withParameter("len", (int)len);
}
