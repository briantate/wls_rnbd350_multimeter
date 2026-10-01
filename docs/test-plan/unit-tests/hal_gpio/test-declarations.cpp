/**
 * @file test-declarations.cpp
 * @brief Test declarations for hal_gpio module (M-07)
 * @date 2026-09-30
 */

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"

extern "C" {
#include "hal/hal_gpio.h"
}

TEST_GROUP(HalGpio)
{
    void setup() override
    {
        mock().clear();
        hal_gpio_init();
    }

    void teardown() override
    {
        mock().checkExpectations();
        mock().clear();
    }
};

/* TCI-065: hal_gpio_init configures pins */
TEST(HalGpio, Init_ConfiguresPins)
{
    // Arrange: MCC driver calls expected
    mock().expectOneCall("GPIO_Initialize");

    // Act
    hal_gpio_init();

    // Assert: MCC init called
}

/* TCI-066: Write to LED pin */
TEST(HalGpio, Write_Led_SetsState)
{
    // Arrange
    mock().expectOneCall("GPIO_PinWrite")
          .withParameter("pin", /* PA15 mapped value */)
          .withParameter("value", true);

    // Act
    hal_gpio_write(HAL_PIN_LED, true);

    // Assert: driver called with correct pin
}

/* TCI-067: Write to RES_A pin */
TEST(HalGpio, Write_ResA_SetsState)
{
    // Arrange
    mock().expectOneCall("GPIO_PinWrite")
          .ignoreOtherParameters();

    // Act
    hal_gpio_write(HAL_PIN_RES_A, true);

    // Assert: verified by mock
}

/* TCI-068: Write to RES_B pin */
TEST(HalGpio, Write_ResB_SetsState)
{
    // Arrange
    mock().expectOneCall("GPIO_PinWrite")
          .ignoreOtherParameters();

    // Act
    hal_gpio_write(HAL_PIN_RES_B, false);

    // Assert: verified by mock
}

/* TCI-069: Write to RES_C pin */
TEST(HalGpio, Write_ResC_SetsState)
{
    // Arrange
    mock().expectOneCall("GPIO_PinWrite")
          .ignoreOtherParameters();

    // Act
    hal_gpio_write(HAL_PIN_RES_C, true);

    // Assert: verified by mock
}

/* TCI-070: Write to SPI_CS pin */
TEST(HalGpio, Write_SpiCs_SetsState)
{
    // Arrange
    mock().expectOneCall("GPIO_PinWrite")
          .ignoreOtherParameters();

    // Act
    hal_gpio_write(HAL_PIN_SPI_CS, false);

    // Assert: verified by mock
}

/* TCI-071: Read returns current state */
TEST(HalGpio, Read_ReturnsCurrentState)
{
    // Arrange
    mock().expectOneCall("GPIO_PinRead")
          .ignoreOtherParameters()
          .andReturnValue(true);

    // Act
    bool state = hal_gpio_read(HAL_PIN_LED);

    // Assert
    CHECK_TRUE(state);
}

/* TCI-072: Toggle inverts state */
TEST(HalGpio, Toggle_InvertsState)
{
    // Arrange
    mock().expectOneCall("GPIO_PinToggle")
          .ignoreOtherParameters();

    // Act
    hal_gpio_toggle(HAL_PIN_LED);

    // Assert: driver toggle called
}

/* TCI-073: Write invalid pin - no effect */
TEST(HalGpio, Write_InvalidPin_NoEffect)
{
    // Arrange: No driver call expected
    mock().expectNoCall("GPIO_PinWrite");

    // Act
    hal_gpio_write((hal_pin_t)99, true);

    // Assert: no crash, no driver call
}

/* TCI-074: Read invalid pin - returns false */
TEST(HalGpio, Read_InvalidPin_ReturnsFalse)
{
    // Arrange: No driver call expected
    mock().expectNoCall("GPIO_PinRead");

    // Act
    bool state = hal_gpio_read((hal_pin_t)99);

    // Assert
    CHECK_FALSE(state);
}
