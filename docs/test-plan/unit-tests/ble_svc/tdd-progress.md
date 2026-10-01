# TDD Progress: ble_svc

**Module:** ble_svc  
**Test Plan:** [test-plan.md](test-plan.md)  
**Test Declarations:** [test-declarations.cpp](test-declarations.cpp)  
**Source File:** `firmware/src/services/ble_svc.c`  
**Test File:** `firmware/tests/ble_svc/test_ble_svc.cpp`

**Started:** 2026-10-01  
**Last Updated:** 2026-10-01  
**Status:** Complete

## Summary

| Category | Total | Implemented | Remaining |
|----------|-------|-------------|-----------|
| ble_svc_init | 1 | 1 | 0 |
| ble_svc_poll - Expected | 10 | 10 | 0 |
| ble_svc_poll - Error | 2 | 2 | 0 |
| ble_svc_transmit_measurement | 6 | 6 | 0 |
| ble_svc_discard_pending | 3 | 3 | 0 |
| **TOTAL** | **22** | **22** | **0** |

## Test Checklist

### ble_svc_init
- [x] TCI-025 `Init_ConfiguresUart` - 2026-10-01

### ble_svc_poll - Expected Behavior
- [x] TCI-026 `Poll_NoData_ReturnsNone` - 2026-10-01
- [x] TCI-027 `Poll_ConnectMessage_ReturnsConnect` - 2026-10-01
- [x] TCI-028 `Poll_DisconnectMessage_ReturnsDisconnect` - 2026-10-01
- [x] TCI-029 `Poll_StreamOpenMessage_ReturnsStreamOpen` - 2026-10-01
- [x] TCI-030 `Poll_PartialMessage_ReturnsNone` - 2026-10-01
- [x] TCI-031 `Poll_FragmentedConnect_EventuallyReturnsConnect` - 2026-10-01
- [x] TCI-032 `Poll_FragmentedDisconnect_EventuallyReturnsDisconnect` - 2026-10-01
- [x] TCI-033 `Poll_FragmentedStreamOpen_EventuallyReturnsStreamOpen` - 2026-10-01
- [x] TCI-034 `Poll_AdjacentMessages_ParsesBoth` - 2026-10-01
- [x] TCI-046 `Poll_ByteAtATime_AccumulatesCorrectly` - 2026-10-01

### ble_svc_poll - Error Handling
- [x] TCI-035 `Poll_GarbageData_IgnoresAndContinues` - 2026-10-01
- [x] TCI-036 `Poll_BufferOverflow_RecoversGracefully` - 2026-10-01

### ble_svc_transmit_measurement
- [x] TCI-037 `TransmitMeasurement_FormatsJson` - 2026-10-01
- [x] TCI-038 `TransmitMeasurement_CorrectJsonStructure` - 2026-10-01
- [x] TCI-039 `TransmitMeasurement_IncludesCrLf` - 2026-10-01
- [x] TCI-040 `TransmitMeasurement_ZeroOhms_FormatsCorrectly` - 2026-10-01
- [x] TCI-041 `TransmitMeasurement_MaxOhms_FormatsCorrectly` - 2026-10-01
- [x] TCI-042 `TransmitMeasurement_SendsViaUart` - 2026-10-01

### ble_svc_discard_pending
- [x] TCI-043 `DiscardPending_ClearsTxBuffer` - 2026-10-01
- [x] TCI-044 `DiscardPending_ClearsRxBuffer` - 2026-10-01
- [x] TCI-045 `DiscardPending_ResetsParserState` - 2026-10-01

## Implementation Notes

### TCI-025: Init_ConfiguresUart
- **Date:** 2026-10-01
- **Function Modified:** `ble_svc_init()`
- **Change:** Implementation already existed - calls hal_uart_init(HAL_UART_BLE)
- **Note:** Test passed immediately; implementation pre-existed in stub file

### TCI-026: Poll_NoData_ReturnsNone
- **Date:** 2026-10-01
- **Function Modified:** `ble_svc_poll()`
- **Change:** Added call to hal_uart_rx_byte() and return BLE_EVENT_NONE

### TCI-027: Poll_ConnectMessage_ReturnsConnect
- **Date:** 2026-10-01
- **Function Modified:** `ble_svc_poll()`
- **Change:** Added message parsing with static buffer to detect "%CONNECT%" and return BLE_EVENT_CONNECT

### TCI-028: Poll_DisconnectMessage_ReturnsDisconnect
- **Date:** 2026-10-01
- **Function Modified:** `ble_svc_poll()`
- **Change:** Added check for "%DISCONNECT%" message

### TCI-029: Poll_StreamOpenMessage_ReturnsStreamOpen
- **Date:** 2026-10-01
- **Function Modified:** `ble_svc_poll()`
- **Change:** Added check for "%STREAM_OPEN%" message

### TCI-030 through TCI-036, TCI-046: Parser enhancements
- **Date:** 2026-10-01
- **Function Modified:** `ble_svc_poll()`
- **Change:** Implemented proper message parser using strstr() to handle:
  - Partial messages (accumulate across calls)
  - Fragmented messages (accumulate bytes)
  - Adjacent messages (parse multiple messages from buffer)
  - Garbage data (search for messages anywhere in buffer)
  - Buffer overflow (wrap at buffer limit)

### TCI-037 through TCI-042: Transmit measurement
- **Date:** 2026-10-01
- **Function Modified:** `ble_svc_transmit_measurement()`
- **Change:** Implemented JSON formatting using snprintf() and transmission via hal_uart_tx_string()
- **Format:** `{"Meter":{"ohm":<value>}}\r\n`

### TCI-043 through TCI-045: Discard pending
- **Date:** 2026-10-01
- **Function Modified:** `ble_svc_discard_pending()`
- **Change:** Clears RX buffer and resets parser state (rx_index = 0, memset buffer)
