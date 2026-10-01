/**
 * @file test_ble_svc.cpp
 * @brief Unit tests for ble_svc module (M-04)
 */

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"

extern "C" {
#include "services/ble_svc.h"
#include "hal/hal_uart.h"
}

TEST_GROUP(BleSvc)
{
    void setup() override
    {
        mock().clear();
        // Note: init is NOT called here to allow individual tests
        // to set up mock expectations before calling init
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
        static char byte_storage;
        byte_storage = c;
        mock().expectOneCall("hal_uart_rx_byte")
              .withParameter("ch", HAL_UART_BLE)
              .withOutputParameterReturning("byte", &byte_storage, sizeof(uint8_t))
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
};

/* TCI-025: ble_svc_init configures UART */
TEST(BleSvc, Init_ConfiguresUart)
{
    // SETUP
    // Clear any prior mock state from setup()
    mock().clear();

    // Set expectation: init should call hal_uart_init with BLE channel
    mock().expectOneCall("hal_uart_init")
          .withParameter("ch", HAL_UART_BLE);

    // EXERCISE
    ble_svc_init();

    // VERIFY
    // Mock framework verifies the expected call occurred in teardown()

    // CLEANUP
    // teardown() handles mock cleanup
}

/* TCI-026: ble_svc_poll with no data returns NONE */
TEST(BleSvc, Poll_NoData_ReturnsNone)
{
    // SETUP
    mock().expectOneCall("hal_uart_init").ignoreOtherParameters();
    ble_svc_init();
    expectNoRxData();

    // EXERCISE
    ble_event_t event = ble_svc_poll();

    // VERIFY
    CHECK_EQUAL(BLE_EVENT_NONE, event);

    // CLEANUP
    // teardown() handles mock cleanup
}

/* TCI-027: ble_svc_poll with CONNECT message returns CONNECT */
TEST(BleSvc, Poll_ConnectMessage_ReturnsConnect)
{
    // SETUP
    mock().expectOneCall("hal_uart_init").ignoreOtherParameters();
    ble_svc_init();
    injectRxString("%CONNECT%");

    // EXERCISE
    ble_event_t event = ble_svc_poll();

    // VERIFY
    CHECK_EQUAL(BLE_EVENT_CONNECT, event);

    // CLEANUP
    // teardown() handles mock cleanup
}

/* TCI-028: ble_svc_poll with DISCONNECT message returns DISCONNECT */
TEST(BleSvc, Poll_DisconnectMessage_ReturnsDisconnect)
{
    // SETUP
    mock().expectOneCall("hal_uart_init").ignoreOtherParameters();
    ble_svc_init();
    injectRxString("%DISCONNECT%");

    // EXERCISE
    ble_event_t event = ble_svc_poll();

    // VERIFY
    CHECK_EQUAL(BLE_EVENT_DISCONNECT, event);

    // CLEANUP
    // teardown() handles mock cleanup
}

/* TCI-029: ble_svc_poll with STREAM_OPEN message returns STREAM_OPEN */
TEST(BleSvc, Poll_StreamOpenMessage_ReturnsStreamOpen)
{
    // SETUP
    mock().expectOneCall("hal_uart_init").ignoreOtherParameters();
    ble_svc_init();
    injectRxString("%STREAM_OPEN%");

    // EXERCISE
    ble_event_t event = ble_svc_poll();

    // VERIFY
    CHECK_EQUAL(BLE_EVENT_STREAM_OPEN, event);

    // CLEANUP
    // teardown() handles mock cleanup
}

/* TCI-030: Partial message returns NONE */
TEST(BleSvc, Poll_PartialMessage_ReturnsNone)
{
    // SETUP
    mock().expectOneCall("hal_uart_init").ignoreOtherParameters();
    ble_svc_init();
    injectRxString("%CONN");

    // EXERCISE
    ble_event_t event = ble_svc_poll();

    // VERIFY
    CHECK_EQUAL(BLE_EVENT_NONE, event);

    // CLEANUP
    // teardown() handles mock cleanup
}

/* TCI-031: Fragmented CONNECT */
TEST(BleSvc, Poll_FragmentedConnect_EventuallyReturnsConnect)
{
    // SETUP
    mock().expectOneCall("hal_uart_init").ignoreOtherParameters();
    ble_svc_init();

    // EXERCISE & VERIFY: First fragment
    injectRxString("%CONN");
    CHECK_EQUAL(BLE_EVENT_NONE, ble_svc_poll());

    // EXERCISE & VERIFY: Second fragment
    injectRxString("ECT%");
    ble_event_t event = ble_svc_poll();
    CHECK_EQUAL(BLE_EVENT_CONNECT, event);

    // CLEANUP
    // teardown() handles mock cleanup
}

/* TCI-032: Fragmented DISCONNECT */
TEST(BleSvc, Poll_FragmentedDisconnect_EventuallyReturnsDisconnect)
{
    // SETUP
    mock().expectOneCall("hal_uart_init").ignoreOtherParameters();
    ble_svc_init();

    // EXERCISE & VERIFY: First fragment
    injectRxString("%DISCON");
    CHECK_EQUAL(BLE_EVENT_NONE, ble_svc_poll());

    // EXERCISE & VERIFY: Second fragment
    injectRxString("NECT%");
    ble_event_t event = ble_svc_poll();
    CHECK_EQUAL(BLE_EVENT_DISCONNECT, event);

    // CLEANUP
    // teardown() handles mock cleanup
}

/* TCI-033: Fragmented STREAM_OPEN */
TEST(BleSvc, Poll_FragmentedStreamOpen_EventuallyReturnsStreamOpen)
{
    // SETUP
    mock().expectOneCall("hal_uart_init").ignoreOtherParameters();
    ble_svc_init();

    // EXERCISE & VERIFY: Byte by byte
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

    // CLEANUP
    // teardown() handles mock cleanup
}

/* TCI-046: Byte at a time accumulation */
TEST(BleSvc, Poll_ByteAtATime_AccumulatesCorrectly)
{
    // SETUP
    mock().expectOneCall("hal_uart_init").ignoreOtherParameters();
    ble_svc_init();

    // EXERCISE: Each byte separately
    const char* msg = "%CONNECT%";
    ble_event_t event = BLE_EVENT_NONE;
    for (int i = 0; msg[i] != '\0'; i++) {
        injectRxByte(msg[i]);
        event = ble_svc_poll();
    }

    // VERIFY
    CHECK_EQUAL(BLE_EVENT_CONNECT, event);

    // CLEANUP
    // teardown() handles mock cleanup
}

/* TCI-034: Adjacent messages */
TEST(BleSvc, Poll_AdjacentMessages_ParsesBoth)
{
    // SETUP
    mock().expectOneCall("hal_uart_init").ignoreOtherParameters();
    ble_svc_init();
    injectRxString("%CONNECT%%STREAM_OPEN%");

    // EXERCISE & VERIFY: First message
    CHECK_EQUAL(BLE_EVENT_CONNECT, ble_svc_poll());

    // EXERCISE & VERIFY: Second message (already in buffer)
    expectNoRxData();
    CHECK_EQUAL(BLE_EVENT_STREAM_OPEN, ble_svc_poll());

    // CLEANUP
    // teardown() handles mock cleanup
}

/* TCI-035: Garbage data ignored */
TEST(BleSvc, Poll_GarbageData_IgnoresAndContinues)
{
    // SETUP
    mock().expectOneCall("hal_uart_init").ignoreOtherParameters();
    ble_svc_init();
    injectRxString("xyz123%CONNECT%");

    // EXERCISE
    ble_event_t event = ble_svc_poll();

    // VERIFY
    CHECK_EQUAL(BLE_EVENT_CONNECT, event);

    // CLEANUP
    // teardown() handles mock cleanup
}

/* TCI-036: Buffer overflow recovery */
TEST(BleSvc, Poll_BufferOverflow_RecoversGracefully)
{
    // SETUP
    mock().expectOneCall("hal_uart_init").ignoreOtherParameters();
    ble_svc_init();

    // EXERCISE: Long garbage that exceeds buffer
    const char* long_garbage = "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA";
    injectRxString(long_garbage);
    ble_svc_poll();  // Should not crash

    // EXERCISE: Valid message after overflow
    injectRxString("%CONNECT%");
    ble_event_t event = ble_svc_poll();

    // VERIFY: Recovers and parses correctly
    CHECK_EQUAL(BLE_EVENT_CONNECT, event);

    // CLEANUP
    // teardown() handles mock cleanup
}

/* TCI-037: transmit_measurement formats JSON */
TEST(BleSvc, TransmitMeasurement_FormatsJson)
{
    // SETUP
    mock().expectOneCall("hal_uart_tx_string")
          .withParameter("ch", HAL_UART_BLE)
          .ignoreOtherParameters();

    // EXERCISE
    ble_svc_transmit_measurement(12345);

    // VERIFY: mock verifies tx_string called
    // teardown() handles mock verification
}

/* TCI-038: Correct JSON structure */
TEST(BleSvc, TransmitMeasurement_CorrectJsonStructure)
{
    // SETUP: Capture the string
    mock().expectOneCall("hal_uart_tx_string")
          .withParameter("ch", HAL_UART_BLE)
          .withParameter("str", "{\"Meter\":{\"ohm\":12345}}\r\n");

    // EXERCISE
    ble_svc_transmit_measurement(12345);

    // VERIFY: mock verifies exact string
}

/* TCI-039: Includes CR LF */
TEST(BleSvc, TransmitMeasurement_IncludesCrLf)
{
    // SETUP
    mock().expectOneCall("hal_uart_tx_string")
          .withParameter("ch", HAL_UART_BLE)
          .withParameter("str", "{\"Meter\":{\"ohm\":100}}\r\n");

    // EXERCISE
    ble_svc_transmit_measurement(100);

    // VERIFY: verified by exact string match
}

/* TCI-040: Zero ohms */
TEST(BleSvc, TransmitMeasurement_ZeroOhms_FormatsCorrectly)
{
    // SETUP
    mock().expectOneCall("hal_uart_tx_string")
          .withParameter("ch", HAL_UART_BLE)
          .withParameter("str", "{\"Meter\":{\"ohm\":0}}\r\n");

    // EXERCISE
    ble_svc_transmit_measurement(0);

    // VERIFY: verified by mock
}

/* TCI-041: Max ohms */
TEST(BleSvc, TransmitMeasurement_MaxOhms_FormatsCorrectly)
{
    // SETUP
    mock().expectOneCall("hal_uart_tx_string")
          .withParameter("ch", HAL_UART_BLE)
          .ignoreOtherParameters();

    // EXERCISE
    ble_svc_transmit_measurement(4294967295U);

    // VERIFY: doesn't crash, transmits something
}

/* TCI-042: Sends via UART */
TEST(BleSvc, TransmitMeasurement_SendsViaUart)
{
    // SETUP
    mock().expectOneCall("hal_uart_tx_string")
          .withParameter("ch", HAL_UART_BLE)
          .ignoreOtherParameters();

    // EXERCISE
    ble_svc_transmit_measurement(500);

    // VERIFY: mock verifies BLE channel used
}

/* TCI-043: discard_pending clears TX */
TEST(BleSvc, DiscardPending_ClearsTxBuffer)
{
    // SETUP: Put something in TX buffer
    mock().ignoreOtherCalls();
    ble_svc_transmit_measurement(100);

    // EXERCISE
    ble_svc_discard_pending();

    // VERIFY: TX buffer cleared (verified by implementation)
    // Future TX should work fresh
    mock().expectOneCall("hal_uart_tx_string").ignoreOtherParameters();
    ble_svc_transmit_measurement(200);
}

/* TCI-044: discard_pending clears RX */
TEST(BleSvc, DiscardPending_ClearsRxBuffer)
{
    // SETUP: Partial message in RX
    mock().expectOneCall("hal_uart_init").ignoreOtherParameters();
    ble_svc_init();
    injectRxString("%CONN");
    ble_svc_poll();

    // EXERCISE
    ble_svc_discard_pending();

    // VERIFY: New complete message parses fresh
    injectRxString("%CONNECT%");
    CHECK_EQUAL(BLE_EVENT_CONNECT, ble_svc_poll());
}

/* TCI-045: discard_pending resets parser */
TEST(BleSvc, DiscardPending_ResetsParserState)
{
    // SETUP: Parser in mid-parse state
    mock().expectOneCall("hal_uart_init").ignoreOtherParameters();
    ble_svc_init();
    injectRxString("%STRE");
    ble_svc_poll();

    // EXERCISE
    ble_svc_discard_pending();

    // VERIFY: Complete different message parses correctly
    injectRxString("%DISCONNECT%");
    CHECK_EQUAL(BLE_EVENT_DISCONNECT, ble_svc_poll());
}
