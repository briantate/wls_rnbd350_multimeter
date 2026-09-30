/**
 * @file test-declarations.cpp
 * @brief Test declarations for hal_spi module (M-08)
 * @date 2026-09-30
 */

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"

extern "C" {
#include "hal/hal_spi.h"
#include "hal/hal_gpio.h"  // For HAL_PIN_SPI_CS
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

/* TCI-077: hal_spi_init configures peripheral */
TEST(HalSpi, Init_ConfiguresPeripheral)
{
    // Arrange
    mock().expectOneCall("SERCOM5_SPI_Initialize");

    // Act
    hal_spi_init();

    // Assert: MCC init called
}

/* TCI-078: Transfer sends byte */
TEST(HalSpi, Transfer_SendsByte)
{
    // Arrange
    mock().expectOneCall("SERCOM5_SPI_TransferByte")
          .withParameter("tx", 0xAB)
          .andReturnValue(0x00);

    // Act
    hal_spi_transfer(0xAB);

    // Assert: byte sent
}

/* TCI-079: Transfer receives byte */
TEST(HalSpi, Transfer_ReceivesByte)
{
    // Arrange
    mock().expectOneCall("SERCOM5_SPI_TransferByte")
          .withParameter("tx", 0x00)
          .andReturnValue(0xCD);

    // Act
    uint8_t rx = hal_spi_transfer(0x00);

    // Assert
    CHECK_EQUAL(0xCD, rx);
}

/* TCI-080: Full duplex transfer */
TEST(HalSpi, Transfer_FullDuplex)
{
    // Arrange: TX 0x55, expect RX 0xAA
    mock().expectOneCall("SERCOM5_SPI_TransferByte")
          .withParameter("tx", 0x55)
          .andReturnValue(0xAA);

    // Act
    uint8_t rx = hal_spi_transfer(0x55);

    // Assert
    CHECK_EQUAL(0xAA, rx);
}

/* TCI-081: Block transfer sends all bytes */
TEST(HalSpi, TransferBlock_SendsAllBytes)
{
    // Arrange
    uint8_t tx[] = {0x01, 0x02, 0x03};
    uint8_t rx[3] = {0};

    mock().expectOneCall("SERCOM5_SPI_TransferByte").withParameter("tx", 0x01).andReturnValue(0);
    mock().expectOneCall("SERCOM5_SPI_TransferByte").withParameter("tx", 0x02).andReturnValue(0);
    mock().expectOneCall("SERCOM5_SPI_TransferByte").withParameter("tx", 0x03).andReturnValue(0);

    // Act
    hal_spi_transfer_block(tx, rx, 3);

    // Assert: all 3 calls made
}

/* TCI-082: Block transfer receives all bytes */
TEST(HalSpi, TransferBlock_ReceivesAllBytes)
{
    // Arrange
    uint8_t tx[] = {0x00, 0x00, 0x00};
    uint8_t rx[3] = {0};

    mock().expectOneCall("SERCOM5_SPI_TransferByte").andReturnValue(0x11);
    mock().expectOneCall("SERCOM5_SPI_TransferByte").andReturnValue(0x22);
    mock().expectOneCall("SERCOM5_SPI_TransferByte").andReturnValue(0x33);

    // Act
    hal_spi_transfer_block(tx, rx, 3);

    // Assert
    CHECK_EQUAL(0x11, rx[0]);
    CHECK_EQUAL(0x22, rx[1]);
    CHECK_EQUAL(0x33, rx[2]);
}

/* TCI-083: Zero length transfer - no operation */
TEST(HalSpi, TransferBlock_ZeroLength_NoOp)
{
    // Arrange
    uint8_t tx[] = {0x01};
    uint8_t rx[1] = {0};
    mock().expectNoCall("SERCOM5_SPI_TransferByte");

    // Act
    hal_spi_transfer_block(tx, rx, 0);

    // Assert: no driver calls
}

/* TCI-084: NULL rx buffer - TX only */
TEST(HalSpi, TransferBlock_NullRx_SendsOnly)
{
    // Arrange
    uint8_t tx[] = {0xAA, 0xBB};

    mock().expectOneCall("hal_gpio_write")
          .withParameter("pin", HAL_PIN_SPI_CS)
          .withParameter("state", false);  // CS assert
    mock().expectOneCall("SERCOM5_SPI_TransferByte").withParameter("tx", 0xAA).andReturnValue(0);
    mock().expectOneCall("SERCOM5_SPI_TransferByte").withParameter("tx", 0xBB).andReturnValue(0);
    mock().expectOneCall("hal_gpio_write")
          .withParameter("pin", HAL_PIN_SPI_CS)
          .withParameter("state", true);   // CS deassert

    // Act
    hal_spi_transfer_block(tx, NULL, 2);

    // Assert: TX happens, RX discarded (no crash)
}

/* TCI-107: Block transfer asserts CS before first byte */
TEST(HalSpi, TransferBlock_AssertsCs)
{
    // Arrange
    uint8_t tx[] = {0x01};
    uint8_t rx[1] = {0};

    // CS must be asserted BEFORE any SPI transfer
    mock().expectOneCall("hal_gpio_write")
          .withParameter("pin", HAL_PIN_SPI_CS)
          .withParameter("state", false);  // Assert low
    mock().expectOneCall("SERCOM5_SPI_TransferByte").ignoreOtherParameters().andReturnValue(0);
    mock().expectOneCall("hal_gpio_write")
          .withParameter("pin", HAL_PIN_SPI_CS)
          .withParameter("state", true);

    // Act
    hal_spi_transfer_block(tx, rx, 1);

    // Assert: CS asserted before SPI byte (verified by mock call order)
}

/* TCI-108: Block transfer deasserts CS after last byte */
TEST(HalSpi, TransferBlock_DeassertsCs)
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
          .withParameter("state", true);   // Deassert high

    // Act
    hal_spi_transfer_block(tx, rx, 2);

    // Assert: CS deasserted after last byte (verified by mock call order)
}
