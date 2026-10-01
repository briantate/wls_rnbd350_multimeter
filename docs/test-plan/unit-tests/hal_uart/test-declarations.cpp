/**
 * @file test-declarations.cpp
 * @brief Test declarations for hal_uart module (M-09)
 * @date 2026-09-30
 */

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"

extern "C" {
#include "hal/hal_uart.h"
}

TEST_GROUP(HalUart)
{
    void setup() override
    {
        mock().clear();
    }

    void teardown() override
    {
        mock().checkExpectations();
        mock().clear();
    }
};

/* TCI-082: Init BLE channel */
TEST(HalUart, Init_Ble_ConfiguresSercom0)
{
    // Arrange
    mock().expectOneCall("SERCOM0_USART_Initialize");

    // Act
    hal_uart_init(HAL_UART_BLE);

    // Assert: MCC init called
}

/* TCI-083: Init DIAG channel */
TEST(HalUart, Init_Diag_ConfiguresSercom2)
{
    // Arrange
    mock().expectOneCall("SERCOM2_USART_Initialize");

    // Act
    hal_uart_init(HAL_UART_DIAG);

    // Assert: MCC init called
}

/* TCI-084: TX ready when buffer empty */
TEST(HalUart, TxReady_WhenReady_ReturnsTrue)
{
    // Arrange
    mock().expectOneCall("SERCOM0_USART_TransmitReady")
          .andReturnValue(true);

    // Act
    bool ready = hal_uart_tx_ready(HAL_UART_BLE);

    // Assert
    CHECK_TRUE(ready);
}

/* TCI-085: TX not ready when buffer full */
TEST(HalUart, TxReady_WhenBusy_ReturnsFalse)
{
    // Arrange
    mock().expectOneCall("SERCOM0_USART_TransmitReady")
          .andReturnValue(false);

    // Act
    bool ready = hal_uart_tx_ready(HAL_UART_BLE);

    // Assert
    CHECK_FALSE(ready);
}

/* TCI-086: TX byte sends to driver */
TEST(HalUart, TxByte_SendsByte)
{
    // Arrange
    mock().expectOneCall("SERCOM0_USART_WriteByte")
          .withParameter("data", 0x55);

    // Act
    hal_uart_tx_byte(HAL_UART_BLE, 0x55);

    // Assert: driver called with byte
}

/* TCI-087: TX string sends all characters */
TEST(HalUart, TxString_SendsAllChars)
{
    // Arrange
    mock().expectOneCall("SERCOM0_USART_WriteByte").withParameter("data", 'H');
    mock().expectOneCall("SERCOM0_USART_WriteByte").withParameter("data", 'i');

    // Act
    hal_uart_tx_string(HAL_UART_BLE, "Hi");

    // Assert: both chars sent
}

/* TCI-088: TX string null-terminated */
TEST(HalUart, TxString_NullTerminated)
{
    // Arrange: Only 3 chars expected, not the null
    mock().expectOneCall("SERCOM0_USART_WriteByte").withParameter("data", 'A');
    mock().expectOneCall("SERCOM0_USART_WriteByte").withParameter("data", 'B');
    mock().expectOneCall("SERCOM0_USART_WriteByte").withParameter("data", 'C');
    mock().expectNoCall("SERCOM0_USART_WriteByte");

    // Act
    hal_uart_tx_string(HAL_UART_BLE, "ABC");

    // Assert: only 3 calls, not 4
}

/* TCI-089: TX empty string - no bytes sent */
TEST(HalUart, TxString_EmptyString_NoOutput)
{
    // Arrange
    mock().expectNoCall("SERCOM0_USART_WriteByte");

    // Act
    hal_uart_tx_string(HAL_UART_BLE, "");

    // Assert: no driver calls
}

/* TCI-090: RX available when data present */
TEST(HalUart, RxAvailable_WhenData_ReturnsTrue)
{
    // Arrange
    mock().expectOneCall("SERCOM0_USART_ReceiverReady")
          .andReturnValue(true);

    // Act
    bool avail = hal_uart_rx_available(HAL_UART_BLE);

    // Assert
    CHECK_TRUE(avail);
}

/* TCI-091: RX not available when empty */
TEST(HalUart, RxAvailable_WhenEmpty_ReturnsFalse)
{
    // Arrange
    mock().expectOneCall("SERCOM0_USART_ReceiverReady")
          .andReturnValue(false);

    // Act
    bool avail = hal_uart_rx_available(HAL_UART_BLE);

    // Assert
    CHECK_FALSE(avail);
}

/* TCI-092: RX byte when data available */
TEST(HalUart, RxByte_WhenData_ReturnsTrueAndOutputsByte)
{
    // Arrange
    mock().expectOneCall("SERCOM0_USART_ReceiverReady")
          .andReturnValue(true);
    mock().expectOneCall("SERCOM0_USART_ReadByte")
          .andReturnValue(0xAA);

    // Act
    uint8_t byte = 0xFF;
    bool result = hal_uart_rx_byte(HAL_UART_BLE, &byte);

    // Assert
    CHECK_TRUE(result);
    CHECK_EQUAL(0xAA, byte);
}

/* TCI-093: RX byte when empty returns false */
TEST(HalUart, RxByte_WhenEmpty_ReturnsFalseByteUnchanged)
{
    // Arrange
    mock().expectOneCall("SERCOM0_USART_ReceiverReady")
          .andReturnValue(false);

    // Act
    uint8_t byte = 0x55;  // Sentinel value
    bool result = hal_uart_rx_byte(HAL_UART_BLE, &byte);

    // Assert
    CHECK_FALSE(result);
    CHECK_EQUAL(0x55, byte);  // Unchanged
}

/* TCI-094: Invalid channel - no effect */
TEST(HalUart, InvalidChannel_NoEffect)
{
    // Arrange
    mock().expectNoCall("SERCOM0_USART_WriteByte");
    mock().expectNoCall("SERCOM2_USART_WriteByte");

    // Act
    hal_uart_tx_byte((hal_uart_channel_t)99, 0x55);

    // Assert: no crash, no driver call
}

/* TCI-095: Both channels independent */
TEST(HalUart, BothChannels_Independent)
{
    // Arrange: Init both
    mock().expectOneCall("SERCOM0_USART_Initialize");
    mock().expectOneCall("SERCOM2_USART_Initialize");
    hal_uart_init(HAL_UART_BLE);
    hal_uart_init(HAL_UART_DIAG);

    mock().clear();

    // Arrange: TX on both
    mock().expectOneCall("SERCOM0_USART_WriteByte").withParameter("data", 'B');
    mock().expectOneCall("SERCOM2_USART_WriteByte").withParameter("data", 'D');

    // Act
    hal_uart_tx_byte(HAL_UART_BLE, 'B');
    hal_uart_tx_byte(HAL_UART_DIAG, 'D');

    // Assert: Each channel uses its own SERCOM
}
