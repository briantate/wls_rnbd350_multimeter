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

/* TCI-085: Init BLE channel */
TEST(HalUart, Init_Ble_ConfiguresSercom0)
{
    // Arrange
    mock().expectOneCall("SERCOM0_USART_Initialize");

    // Act
    hal_uart_init(HAL_UART_BLE);

    // Assert: MCC init called
}

/* TCI-086: Init DIAG channel */
TEST(HalUart, Init_Diag_ConfiguresSercom2)
{
    // Arrange
    mock().expectOneCall("SERCOM2_USART_Initialize");

    // Act
    hal_uart_init(HAL_UART_DIAG);

    // Assert: MCC init called
}

/* TCI-087: TX ready when buffer empty */
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

/* TCI-088: TX not ready when buffer full */
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

/* TCI-089: TX byte sends to driver */
TEST(HalUart, TxByte_SendsByte)
{
    // Arrange
    mock().expectOneCall("SERCOM0_USART_WriteByte")
          .withParameter("data", 0x55);

    // Act
    hal_uart_tx_byte(HAL_UART_BLE, 0x55);

    // Assert: driver called with byte
}

/* TCI-090: TX string sends all characters */
TEST(HalUart, TxString_SendsAllChars)
{
    // Arrange
    mock().expectOneCall("SERCOM0_USART_WriteByte").withParameter("data", 'H');
    mock().expectOneCall("SERCOM0_USART_WriteByte").withParameter("data", 'i');

    // Act
    hal_uart_tx_string(HAL_UART_BLE, "Hi");

    // Assert: both chars sent
}

/* TCI-091: TX string null-terminated */
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

/* TCI-092: TX empty string - no bytes sent */
TEST(HalUart, TxString_EmptyString_NoOutput)
{
    // Arrange
    mock().expectNoCall("SERCOM0_USART_WriteByte");

    // Act
    hal_uart_tx_string(HAL_UART_BLE, "");

    // Assert: no driver calls
}

/* TCI-093: RX available when data present */
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

/* TCI-094: RX not available when empty */
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

/* TCI-095: RX byte returns data */
TEST(HalUart, RxByte_ReturnsByte)
{
    // Arrange
    mock().expectOneCall("SERCOM0_USART_ReadByte")
          .andReturnValue(0xAA);

    // Act
    uint8_t byte = hal_uart_rx_byte(HAL_UART_BLE);

    // Assert
    CHECK_EQUAL(0xAA, byte);
}

/* TCI-096: RX byte when empty returns 0 */
TEST(HalUart, RxByte_WhenEmpty_ReturnsZero)
{
    // Arrange: Simulate empty buffer
    mock().expectOneCall("SERCOM0_USART_ReceiverReady")
          .andReturnValue(false);

    // Act: Implementation should check availability first
    // or return 0 if no data
    uint8_t byte = hal_uart_rx_byte(HAL_UART_BLE);

    // Assert
    CHECK_EQUAL(0, byte);
}

/* TCI-097: Invalid channel - no effect */
TEST(HalUart, InvalidChannel_NoEffect)
{
    // Arrange
    mock().expectNoCall("SERCOM0_USART_WriteByte");
    mock().expectNoCall("SERCOM2_USART_WriteByte");

    // Act
    hal_uart_tx_byte((hal_uart_channel_t)99, 0x55);

    // Assert: no crash, no driver call
}

/* TCI-098: Both channels independent */
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
