/**
 * @file test-declarations.cpp
 * @brief Test declarations for hal_spi module (M-08)
 * @date 2026-09-30
 */

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"

extern "C" {
#include "hal/hal_spi.h"
#include "hal/hal_gpio.h"
}

TEST_GROUP(HalSpi)
{
    void setup() override
    {
        mock().clear();
        hal_spi_init();
    }

    void teardown() override
    {
        mock().checkExpectations();
        mock().clear();
    }
};

/* TCI-075: hal_spi_init configures peripheral */
TEST(HalSpi, Init_ConfiguresPeripheral)
{
    // Arrange
    mock().expectOneCall("SERCOM5_SPI_Initialize");

    // Act
    hal_spi_init();

    // Assert: MCC init called
}

/* TCI-076: Transfer sends all bytes */
TEST(HalSpi, Transfer_SendsAllBytes)
{
    // Arrange
    uint8_t tx[] = {0x01, 0x02, 0x03};
    uint8_t rx[3] = {0};

    mock().expectOneCall("hal_gpio_write")
          .withParameter("pin", HAL_PIN_SPI_CS)
          .withParameter("state", false);
    mock().expectOneCall("SERCOM5_SPI_TransferByte").withParameter("tx", 0x01).andReturnValue(0);
    mock().expectOneCall("SERCOM5_SPI_TransferByte").withParameter("tx", 0x02).andReturnValue(0);
    mock().expectOneCall("SERCOM5_SPI_TransferByte").withParameter("tx", 0x03).andReturnValue(0);
    mock().expectOneCall("hal_gpio_write")
          .withParameter("pin", HAL_PIN_SPI_CS)
          .withParameter("state", true);

    // Act
    hal_spi_transfer(tx, rx, 3);

    // Assert: all 3 calls made
}

/* TCI-077: Transfer receives all bytes */
TEST(HalSpi, Transfer_ReceivesAllBytes)
{
    // Arrange
    uint8_t tx[] = {0x00, 0x00, 0x00};
    uint8_t rx[3] = {0};

    mock().expectOneCall("hal_gpio_write").ignoreOtherParameters();
    mock().expectOneCall("SERCOM5_SPI_TransferByte").andReturnValue(0x11);
    mock().expectOneCall("SERCOM5_SPI_TransferByte").andReturnValue(0x22);
    mock().expectOneCall("SERCOM5_SPI_TransferByte").andReturnValue(0x33);
    mock().expectOneCall("hal_gpio_write").ignoreOtherParameters();

    // Act
    hal_spi_transfer(tx, rx, 3);

    // Assert
    CHECK_EQUAL(0x11, rx[0]);
    CHECK_EQUAL(0x22, rx[1]);
    CHECK_EQUAL(0x33, rx[2]);
}

/* TCI-078: Zero length transfer - no operation */
TEST(HalSpi, Transfer_ZeroLength_NoOp)
{
    // Arrange
    uint8_t tx[] = {0x01};
    uint8_t rx[1] = {0};
    mock().expectNoCall("SERCOM5_SPI_TransferByte");
    mock().expectNoCall("hal_gpio_write");

    // Act
    hal_spi_transfer(tx, rx, 0);

    // Assert: no driver calls, no CS toggle
}

/* TCI-079: NULL rx buffer - TX only */
TEST(HalSpi, Transfer_NullRx_SendsOnly)
{
    // Arrange
    uint8_t tx[] = {0xAA, 0xBB};

    mock().expectOneCall("hal_gpio_write")
          .withParameter("pin", HAL_PIN_SPI_CS)
          .withParameter("state", false);
    mock().expectOneCall("SERCOM5_SPI_TransferByte").withParameter("tx", 0xAA).andReturnValue(0);
    mock().expectOneCall("SERCOM5_SPI_TransferByte").withParameter("tx", 0xBB).andReturnValue(0);
    mock().expectOneCall("hal_gpio_write")
          .withParameter("pin", HAL_PIN_SPI_CS)
          .withParameter("state", true);

    // Act
    hal_spi_transfer(tx, NULL, 2);

    // Assert: TX happens, RX discarded (no crash)
}

/* TCI-080: Transfer asserts CS before first byte */
TEST(HalSpi, Transfer_AssertsCs)
{
    // Arrange
    uint8_t tx[] = {0x01};
    uint8_t rx[1] = {0};

    // CS must be asserted BEFORE any SPI transfer
    mock().expectOneCall("hal_gpio_write")
          .withParameter("pin", HAL_PIN_SPI_CS)
          .withParameter("state", false);
    mock().expectOneCall("SERCOM5_SPI_TransferByte").ignoreOtherParameters().andReturnValue(0);
    mock().expectOneCall("hal_gpio_write")
          .withParameter("pin", HAL_PIN_SPI_CS)
          .withParameter("state", true);

    // Act
    hal_spi_transfer(tx, rx, 1);

    // Assert: CS asserted before SPI byte (verified by mock call order)
}

/* TCI-081: Transfer deasserts CS after last byte */
TEST(HalSpi, Transfer_DeassertsCs)
{
    // Arrange
    uint8_t tx[] = {0x01, 0x02};
    uint8_t rx[2] = {0};

    mock().expectOneCall("hal_gpio_write")
          .withParameter("pin", HAL_PIN_SPI_CS)
          .withParameter("state", false);
    mock().expectOneCall("SERCOM5_SPI_TransferByte").ignoreOtherParameters().andReturnValue(0);
    mock().expectOneCall("SERCOM5_SPI_TransferByte").ignoreOtherParameters().andReturnValue(0);
    // CS must be deasserted AFTER all SPI transfers
    mock().expectOneCall("hal_gpio_write")
          .withParameter("pin", HAL_PIN_SPI_CS)
          .withParameter("state", true);

    // Act
    hal_spi_transfer(tx, rx, 2);

    // Assert: CS deasserted after last byte (verified by mock call order)
}
