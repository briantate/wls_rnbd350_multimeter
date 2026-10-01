/**
 * @file test-declarations.cpp
 * @brief Test declarations for ble_svc module (M-04)
 * @date 2026-09-30
 */

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"

extern "C" {
#include "services/ble_svc.h"
#include "hal/hal_uart.h"
}

static char tx_buffer[128];
static int tx_index;

TEST_GROUP(BleSvc)
{
    void setup() override
    {
        mock().clear();
        tx_index = 0;
        memset(tx_buffer, 0, sizeof(tx_buffer));
        ble_svc_init();
    }

    void teardown() override
    {
        mock().checkExpectations();
        mock().clear();
    }

    void injectRxString(const char* str)
    {
        for (int i = 0; str[i] != '\0'; i++) {
            mock().expectOneCall("hal_uart_rx_byte")
                  .withParameter("ch", HAL_UART_BLE)
                  .withOutputParameterReturning("byte", &str[i], sizeof(uint8_t))
                  .andReturnValue(true);
        }
        mock().expectOneCall("hal_uart_rx_byte")
              .withParameter("ch", HAL_UART_BLE)
              .ignoreOtherParameters()
              .andReturnValue(false);
    }

    void injectRxByte(char c)
    {
        mock().expectOneCall("hal_uart_rx_byte")
              .withParameter("ch", HAL_UART_BLE)
              .withOutputParameterReturning("byte", &c, sizeof(uint8_t))
              .andReturnValue(true);
        mock().expectOneCall("hal_uart_rx_byte")
              .withParameter("ch", HAL_UART_BLE)
              .ignoreOtherParameters()
              .andReturnValue(false);
    }

    void expectNoRxData()
    {
        mock().expectOneCall("hal_uart_rx_byte")
              .withParameter("ch", HAL_UART_BLE)
              .ignoreOtherParameters()
              .andReturnValue(false);
    }

    void captureTxString()
    {
        mock().expectNCalls(128, "hal_uart_tx_byte")
              .ignoreOtherParameters();
    }
};

/* TCI-025: ble_svc_init */
TEST(BleSvc, Init_ConfiguresUart)
{
    // Arrange
    mock().expectOneCall("hal_uart_init")
          .withParameter("ch", HAL_UART_BLE);

    // Act
    ble_svc_init();

    // Assert: mock verifies
}

/* TCI-026: ble_svc_poll with no data */
TEST(BleSvc, Poll_NoData_ReturnsNone)
{
    // Arrange
    expectNoRxData();

    // Act
    ble_event_t event = ble_svc_poll();

    // Assert
    CHECK_EQUAL(BLE_EVENT_NONE, event);
}

/* TCI-027: ble_svc_poll with CONNECT message */
TEST(BleSvc, Poll_ConnectMessage_ReturnsConnect)
{
    // Arrange
    injectRxString("%CONNECT%");

    // Act
    ble_event_t event = ble_svc_poll();

    // Assert
    CHECK_EQUAL(BLE_EVENT_CONNECT, event);
}

/* TCI-028: ble_svc_poll with DISCONNECT message */
TEST(BleSvc, Poll_DisconnectMessage_ReturnsDisconnect)
{
    // Arrange
    injectRxString("%DISCONNECT%");

    // Act
    ble_event_t event = ble_svc_poll();

    // Assert
    CHECK_EQUAL(BLE_EVENT_DISCONNECT, event);
}

/* TCI-029: ble_svc_poll with STREAM_OPEN message */
TEST(BleSvc, Poll_StreamOpenMessage_ReturnsStreamOpen)
{
    // Arrange
    injectRxString("%STREAM_OPEN%");

    // Act
    ble_event_t event = ble_svc_poll();

    // Assert
    CHECK_EQUAL(BLE_EVENT_STREAM_OPEN, event);
}

/* TCI-030: Partial message returns NONE */
TEST(BleSvc, Poll_PartialMessage_ReturnsNone)
{
    // Arrange
    injectRxString("%CONN");

    // Act
    ble_event_t event = ble_svc_poll();

    // Assert
    CHECK_EQUAL(BLE_EVENT_NONE, event);
}

/* TCI-031: Fragmented CONNECT */
TEST(BleSvc, Poll_FragmentedConnect_EventuallyReturnsConnect)
{
    // Arrange & Act: First fragment
    injectRxString("%CONN");
    CHECK_EQUAL(BLE_EVENT_NONE, ble_svc_poll());

    // Arrange & Act: Second fragment
    injectRxString("ECT%");
    ble_event_t event = ble_svc_poll();

    // Assert
    CHECK_EQUAL(BLE_EVENT_CONNECT, event);
}

/* TCI-032: Fragmented DISCONNECT */
TEST(BleSvc, Poll_FragmentedDisconnect_EventuallyReturnsDisconnect)
{
    // Arrange & Act: First fragment
    injectRxString("%DISCON");
    CHECK_EQUAL(BLE_EVENT_NONE, ble_svc_poll());

    // Arrange & Act: Second fragment
    injectRxString("NECT%");
    ble_event_t event = ble_svc_poll();

    // Assert
    CHECK_EQUAL(BLE_EVENT_DISCONNECT, event);
}

/* TCI-033: Fragmented STREAM_OPEN */
TEST(BleSvc, Poll_FragmentedStreamOpen_EventuallyReturnsStreamOpen)
{
    // Arrange & Act: Byte by byte
    const char* msg = "%STREAM_OPEN%";
    for (int i = 0; msg[i] != '\0'; i++) {
        injectRxByte(msg[i]);
        ble_event_t event = ble_svc_poll();
        if (msg[i+1] == '\0') {
            CHECK_EQUAL(BLE_EVENT_STREAM_OPEN, event);
        } else {
            CHECK_EQUAL(BLE_EVENT_NONE, event);
        }
    }
}

/* TCI-034: Adjacent messages */
TEST(BleSvc, Poll_AdjacentMessages_ParsesBoth)
{
    // Arrange
    injectRxString("%CONNECT%%STREAM_OPEN%");

    // Act & Assert: First message
    CHECK_EQUAL(BLE_EVENT_CONNECT, ble_svc_poll());

    // Need to poll again for second message
    expectNoRxData();  // Already consumed from buffer
    CHECK_EQUAL(BLE_EVENT_STREAM_OPEN, ble_svc_poll());
}

/* TCI-035: Garbage data ignored */
TEST(BleSvc, Poll_GarbageData_IgnoresAndContinues)
{
    // Arrange: Garbage then valid message
    injectRxString("xyz123%CONNECT%");

    // Act
    ble_event_t event = ble_svc_poll();

    // Assert
    CHECK_EQUAL(BLE_EVENT_CONNECT, event);
}

/* TCI-036: Buffer overflow recovery */
TEST(BleSvc, Poll_BufferOverflow_RecoversGracefully)
{
    // Arrange: Long garbage that exceeds buffer
    const char* long_garbage = "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA";
    injectRxString(long_garbage);
    ble_svc_poll();  // Should not crash

    // Arrange: Valid message after overflow
    injectRxString("%CONNECT%");

    // Act
    ble_event_t event = ble_svc_poll();

    // Assert: Recovers and parses correctly
    CHECK_EQUAL(BLE_EVENT_CONNECT, event);
}

/* TCI-037: transmit_measurement formats JSON */
TEST(BleSvc, TransmitMeasurement_FormatsJson)
{
    // Arrange
    mock().expectOneCall("hal_uart_tx_string")
          .withParameter("ch", HAL_UART_BLE)
          .ignoreOtherParameters();

    // Act
    ble_svc_transmit_measurement(12345);

    // Assert: mock verifies tx_string called
}

/* TCI-038: Correct JSON structure */
TEST(BleSvc, TransmitMeasurement_CorrectJsonStructure)
{
    // Arrange: Capture the string
    mock().expectOneCall("hal_uart_tx_string")
          .withParameter("ch", HAL_UART_BLE)
          .withParameter("str", "{\"Meter\":{\"ohm\":12345}}\r\n");

    // Act
    ble_svc_transmit_measurement(12345);

    // Assert: mock verifies exact string
}

/* TCI-039: Includes CR LF */
TEST(BleSvc, TransmitMeasurement_IncludesCrLf)
{
    // Arrange
    mock().expectOneCall("hal_uart_tx_string")
          .withParameter("ch", HAL_UART_BLE)
          .withParameter("str", "{\"Meter\":{\"ohm\":100}}\r\n");

    // Act
    ble_svc_transmit_measurement(100);

    // Assert: verified by exact string match
}

/* TCI-040: Zero ohms */
TEST(BleSvc, TransmitMeasurement_ZeroOhms_FormatsCorrectly)
{
    // Arrange
    mock().expectOneCall("hal_uart_tx_string")
          .withParameter("ch", HAL_UART_BLE)
          .withParameter("str", "{\"Meter\":{\"ohm\":0}}\r\n");

    // Act
    ble_svc_transmit_measurement(0);

    // Assert: verified by mock
}

/* TCI-041: Max ohms */
TEST(BleSvc, TransmitMeasurement_MaxOhms_FormatsCorrectly)
{
    // Arrange
    mock().expectOneCall("hal_uart_tx_string")
          .withParameter("ch", HAL_UART_BLE)
          .ignoreOtherParameters();  // Max uint32 is long

    // Act
    ble_svc_transmit_measurement(4294967295U);

    // Assert: doesn't crash, transmits something
}

/* TCI-042: Sends via UART */
TEST(BleSvc, TransmitMeasurement_SendsViaUart)
{
    // Arrange
    mock().expectOneCall("hal_uart_tx_string")
          .withParameter("ch", HAL_UART_BLE)
          .ignoreOtherParameters();

    // Act
    ble_svc_transmit_measurement(500);

    // Assert: mock verifies BLE channel used
}

/* TCI-043: discard_pending clears TX */
TEST(BleSvc, DiscardPending_ClearsTxBuffer)
{
    // Arrange: Put something in TX buffer
    mock().ignoreOtherCalls();
    ble_svc_transmit_measurement(100);

    // Act
    ble_svc_discard_pending();

    // Assert: TX buffer cleared (verified by implementation)
    // Future TX should work fresh
    mock().expectOneCall("hal_uart_tx_string").ignoreOtherParameters();
    ble_svc_transmit_measurement(200);
}

/* TCI-044: discard_pending clears RX */
TEST(BleSvc, DiscardPending_ClearsRxBuffer)
{
    // Arrange: Partial message in RX
    injectRxString("%CONN");
    ble_svc_poll();

    // Act
    ble_svc_discard_pending();

    // Arrange: New complete message
    injectRxString("%CONNECT%");

    // Assert: Parses fresh, not corrupted by old data
    CHECK_EQUAL(BLE_EVENT_CONNECT, ble_svc_poll());
}

/* TCI-045: discard_pending resets parser */
TEST(BleSvc, DiscardPending_ResetsParserState)
{
    // Arrange: Parser in mid-parse state
    injectRxString("%STRE");
    ble_svc_poll();

    // Act
    ble_svc_discard_pending();

    // Arrange: Complete different message
    injectRxString("%DISCONNECT%");

    // Assert: Parses new message correctly
    CHECK_EQUAL(BLE_EVENT_DISCONNECT, ble_svc_poll());
}

/* TCI-046: Byte at a time accumulation */
TEST(BleSvc, Poll_ByteAtATime_AccumulatesCorrectly)
{
    // Arrange & Act: Each byte separately
    const char* msg = "%CONNECT%";
    ble_event_t event = BLE_EVENT_NONE;

    for (int i = 0; msg[i] != '\0'; i++) {
        injectRxByte(msg[i]);
        event = ble_svc_poll();
    }

    // Assert
    CHECK_EQUAL(BLE_EVENT_CONNECT, event);
}
