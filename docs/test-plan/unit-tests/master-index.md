# Master Test Index

**Project:** Bluetooth Ohmmeter Firmware  
**Version:** 1.2  
**Date:** 2026-09-30  
**Status:** Draft

---

## 1. Overview

This document indexes all unit tests across all modules in the Bluetooth Ohmmeter firmware. Each test is uniquely identified, traced to requirements, and linked to its module test plan.

---

## 2. Test Naming Convention

```
<ModuleName>_<FunctionUnderTest>_<Scenario>_<ExpectedResult>
```

Example: `AppState_OnConnect_WhenDisconnected_TransitionsToConnected`

---

## 3. Test Summary by Module

| Module | ID | Test Count | Coverage Target | Status |
|--------|-----|------------|-----------------|--------|
| app_state | M-02 | 12 | 100% branch | Planned |
| measurement_svc | M-03 | 12 | 100% branch | Planned |
| ble_svc | M-04 | 22 | 100% branch | Planned |
| led_svc | M-05 | 10 | 100% branch | Planned |
| diag_svc | M-06 | 8 | 100% branch | Planned |
| hal_gpio | M-07 | 10 | 100% branch | Planned |
| hal_spi | M-08 | 7 | 100% branch | Planned |
| hal_uart | M-09 | 14 | 100% branch | Planned |
| hal_tick | M-10 | 4 | 100% branch | Planned |
| **Total** | | **99** | | |

> **Note:** M-01 (main) is integration-tested, not unit-tested. Its logic is minimal (init + super-loop).

---

## 4. Test Index by Module

### 4.1 M-02: app_state

| Test ID | Test Name | Traces To |
|---------|-----------|-----------|
| TCI-001 | AppState_Init_SetsDisconnected | E2-006 |
| TCI-002 | AppState_Get_ReturnsCurrentState | E2-004 |
| TCI-003 | AppState_OnConnect_WhenDisconnected_TransitionsToConnected | E2-004 |
| TCI-004 | AppState_OnConnect_WhenConnected_RemainsConnected | E2-004 |
| TCI-005 | AppState_OnConnect_WhenStreaming_RemainsStreaming | E2-004 |
| TCI-006 | AppState_OnStreamOpen_WhenConnected_TransitionsToStreaming | E2-004 |
| TCI-007 | AppState_OnStreamOpen_WhenDisconnected_RemainsDisconnected | E2-004 |
| TCI-008 | AppState_OnStreamOpen_WhenStreaming_RemainsStreaming | E2-004 |
| TCI-009 | AppState_OnDisconnect_WhenConnected_TransitionsToDisconnected | E2-006 |
| TCI-010 | AppState_OnDisconnect_WhenStreaming_TransitionsToDisconnected | E2-006 |
| TCI-011 | AppState_IsStreaming_WhenStreaming_ReturnsTrue | E2-004 |
| TCI-012 | AppState_IsStreaming_WhenNotStreaming_ReturnsFalse | E2-004 |

**Test Plan:** [app_state/test-plan.md](app_state/test-plan.md)

---

### 4.2 M-03: measurement_svc

| Test ID | Test Name | Traces To |
|---------|-----------|-----------|
| TCI-013 | MeasurementSvc_Init_ConfiguresRangeGpios | E1-004 |
| TCI-014 | MeasurementSvc_Sample_ReadsAdcViaHal | E1-002 |
| TCI-015 | MeasurementSvc_Sample_ConvertsCountsToOhms | E1-003 |
| TCI-016 | MeasurementSvc_Sample_ReturnsValidMeasurement | E1-002 |
| TCI-017 | MeasurementSvc_Sample_ZeroCounts_ReturnsZeroOhms | E1-003 |
| TCI-018 | MeasurementSvc_Sample_MaxCounts_ReturnsMaxOhms | E1-003 |
| TCI-019 | MeasurementSvc_Sample_MidCounts_ReturnsCorrectOhms | E1-003 |
| TCI-020 | MeasurementSvc_Sample_SendsCorrectSpiCommand | E1-002 |
| TCI-021 | MeasurementSvc_Sample_Channel2Selected | E1-002 |
| TCI-022 | MeasurementSvc_Sample_SpiError_ReturnsInvalid | E1-002 |
| TCI-023 | MeasurementSvc_Sample_MultipleConsecutive_WorksCorrectly | E1-002 |
| TCI-024 | MeasurementSvc_Sample_TimingWithinBudget | E5-002 |

**Test Plan:** [measurement_svc/test-plan.md](measurement_svc/test-plan.md)

---

### 4.3 M-04: ble_svc

| Test ID | Test Name | Traces To |
|---------|-----------|-----------|
| TCI-025 | BleSvc_Init_ConfiguresUart | E2-002 |
| TCI-026 | BleSvc_Poll_NoData_ReturnsNone | E2-003 |
| TCI-027 | BleSvc_Poll_ConnectMessage_ReturnsConnect | E2-003 |
| TCI-028 | BleSvc_Poll_DisconnectMessage_ReturnsDisconnect | E2-003 |
| TCI-029 | BleSvc_Poll_StreamOpenMessage_ReturnsStreamOpen | E2-004 |
| TCI-030 | BleSvc_Poll_PartialMessage_ReturnsNone | E2-003 |
| TCI-031 | BleSvc_Poll_FragmentedConnect_EventuallyReturnsConnect | E2-003 |
| TCI-032 | BleSvc_Poll_FragmentedDisconnect_EventuallyReturnsDisconnect | E2-003 |
| TCI-033 | BleSvc_Poll_FragmentedStreamOpen_EventuallyReturnsStreamOpen | E2-004 |
| TCI-034 | BleSvc_Poll_AdjacentMessages_ParsesBoth | E2-003 |
| TCI-035 | BleSvc_Poll_GarbageData_IgnoresAndContinues | E2-003 |
| TCI-036 | BleSvc_Poll_BufferOverflow_RecoversGracefully | E2-003 |
| TCI-037 | BleSvc_TransmitMeasurement_FormatsJson | E2-005 |
| TCI-038 | BleSvc_TransmitMeasurement_CorrectJsonStructure | E2-005 |
| TCI-039 | BleSvc_TransmitMeasurement_IncludesCrLf | E2-005 |
| TCI-040 | BleSvc_TransmitMeasurement_ZeroOhms_FormatsCorrectly | E2-005 |
| TCI-041 | BleSvc_TransmitMeasurement_MaxOhms_FormatsCorrectly | E2-005 |
| TCI-042 | BleSvc_TransmitMeasurement_SendsViaUart | E2-005 |
| TCI-043 | BleSvc_DiscardPending_ClearsTxBuffer | E2-006 |
| TCI-044 | BleSvc_DiscardPending_ClearsRxBuffer | E2-006 |
| TCI-045 | BleSvc_DiscardPending_ResetsParserState | E2-006 |
| TCI-046 | BleSvc_Poll_ByteAtATime_AccumulatesCorrectly | E2-003 |

**Test Plan:** [ble_svc/test-plan.md](ble_svc/test-plan.md)

---

### 4.4 M-05: led_svc

| Test ID | Test Name | Traces To |
|---------|-----------|-----------|
| TCI-047 | LedSvc_Init_TurnsLedOff | E3-002 |
| TCI-048 | LedSvc_Update_WhenDisconnected_TogglesAt1000ms | E3-002 |
| TCI-049 | LedSvc_Update_WhenStreaming_TogglesAt200ms | E3-003 |
| TCI-050 | LedSvc_Update_Before1000ms_NoToggle | E3-002 |
| TCI-051 | LedSvc_Update_At1000ms_Toggles | E3-002 |
| TCI-052 | LedSvc_Update_Before200ms_NoToggle | E3-003 |
| TCI-053 | LedSvc_Update_At200ms_Toggles | E3-003 |
| TCI-054 | LedSvc_Update_StateChangesMidCycle_AdjustsRate | E3-002, E3-003 |
| TCI-055 | LedSvc_Update_AccumulatesElapsedTime | E3-002 |
| TCI-056 | LedSvc_Update_ResetsAfterToggle | E3-002 |

**Test Plan:** [led_svc/test-plan.md](led_svc/test-plan.md)

---

### 4.5 M-06: diag_svc

| Test ID | Test Name | Traces To |
|---------|-----------|-----------|
| TCI-057 | DiagSvc_Init_ConfiguresUart | E4-002 |
| TCI-058 | DiagSvc_Log_FormatsTimestamp | E4-002 |
| TCI-059 | DiagSvc_Log_FormatsLevel | E4-002 |
| TCI-060 | DiagSvc_Log_FormatsSubsystem | E4-002 |
| TCI-061 | DiagSvc_Log_FormatsMessage | E4-002 |
| TCI-062 | DiagSvc_Log_BelowVerbosity_NoOutput | E4-003 |
| TCI-063 | DiagSvc_Log_AtVerbosity_Outputs | E4-003 |
| TCI-064 | DiagSvc_Log_AboveVerbosity_Outputs | E4-003 |

**Test Plan:** [diag_svc/test-plan.md](diag_svc/test-plan.md)

---

### 4.6 M-07: hal_gpio

| Test ID | Test Name | Traces To |
|---------|-----------|-----------|
| TCI-065 | HalGpio_Init_ConfiguresPins | E1-004, E3-002 |
| TCI-066 | HalGpio_Write_Led_SetsState | E3-002 |
| TCI-067 | HalGpio_Write_ResA_SetsState | E1-004 |
| TCI-068 | HalGpio_Write_ResB_SetsState | E1-004 |
| TCI-069 | HalGpio_Write_ResC_SetsState | E1-004 |
| TCI-070 | HalGpio_Write_SpiCs_SetsState | E1-002 |
| TCI-071 | HalGpio_Read_ReturnsCurrentState | E1-004 |
| TCI-072 | HalGpio_Toggle_InvertsState | E3-002 |
| TCI-073 | HalGpio_Write_InvalidPin_NoEffect | - |
| TCI-074 | HalGpio_Read_InvalidPin_ReturnsFalse | - |

**Test Plan:** [hal_gpio/test-plan.md](hal_gpio/test-plan.md)

---

### 4.7 M-08: hal_spi

| Test ID | Test Name | Traces To |
|---------|-----------|-----------|
| TCI-075 | HalSpi_Init_ConfiguresPeripheral | E1-002 |
| TCI-076 | HalSpi_Transfer_SendsAllBytes | E1-002 |
| TCI-077 | HalSpi_Transfer_ReceivesAllBytes | E1-002 |
| TCI-078 | HalSpi_Transfer_ZeroLength_NoOp | E1-002 |
| TCI-079 | HalSpi_Transfer_NullRx_SendsOnly | E1-002 |
| TCI-080 | HalSpi_Transfer_AssertsCs | E1-002 |
| TCI-081 | HalSpi_Transfer_DeassertsCs | E1-002 |

**Test Plan:** [hal_spi/test-plan.md](hal_spi/test-plan.md)

---

### 4.8 M-09: hal_uart

| Test ID | Test Name | Traces To |
|---------|-----------|-----------|
| TCI-082 | HalUart_Init_Ble_ConfiguresSercom0 | E2-002 |
| TCI-083 | HalUart_Init_Diag_ConfiguresSercom2 | E4-002 |
| TCI-084 | HalUart_TxReady_WhenReady_ReturnsTrue | E2-005 |
| TCI-085 | HalUart_TxReady_WhenBusy_ReturnsFalse | E2-005 |
| TCI-086 | HalUart_TxByte_SendsByte | E2-005 |
| TCI-087 | HalUart_TxString_SendsAllChars | E2-005 |
| TCI-088 | HalUart_TxString_NullTerminated | E2-005 |
| TCI-089 | HalUart_TxString_EmptyString_NoOutput | E2-005 |
| TCI-090 | HalUart_RxAvailable_WhenData_ReturnsTrue | E2-003 |
| TCI-091 | HalUart_RxAvailable_WhenEmpty_ReturnsFalse | E2-003 |
| TCI-092 | HalUart_RxByte_WhenData_ReturnsTrueAndOutputsByte | E2-003 |
| TCI-093 | HalUart_RxByte_WhenEmpty_ReturnsFalseByteUnchanged | E2-003 |
| TCI-094 | HalUart_InvalidChannel_NoEffect | - |
| TCI-095 | HalUart_BothChannels_Independent | E2-002, E4-002 |

**Test Plan:** [hal_uart/test-plan.md](hal_uart/test-plan.md)

---

### 4.9 M-10: hal_tick

| Test ID | Test Name | Traces To |
|---------|-----------|-----------|
| TCI-096 | HalTick_Init_StartsTimer | E5-002 |
| TCI-097 | HalTick_GetMs_ReturnsMilliseconds | E5-002 |
| TCI-098 | HalTick_GetMs_IncrementsOverTime | E5-002 |
| TCI-099 | HalTick_GetMs_WraparoundSafe | E5-002 |

**Test Plan:** [hal_tick/test-plan.md](hal_tick/test-plan.md)

---

## 5. Traceability Matrix

| Enabler Story | Test IDs | Coverage |
|---------------|----------|----------|
| E1-002 (Read ADC) | TCI-014, TCI-016, TCI-020-024, TCI-070, TCI-075-081 | ✓ Full |
| E1-003 (Convert to ohms) | TCI-015, TCI-017-019 | ✓ Full |
| E1-004 (Configure range) | TCI-013, TCI-065, TCI-067-069, TCI-071 | ✓ Full |
| E2-002 (UART init) | TCI-025, TCI-082-083, TCI-095 | ✓ Full |
| E2-003 (Status parsing) | TCI-026-036, TCI-046, TCI-090-093 | ✓ Full |
| E2-004 (STREAM_OPEN) | TCI-002-008, TCI-011-012, TCI-029, TCI-033 | ✓ Full |
| E2-005 (JSON TX) | TCI-037-042, TCI-084-089 | ✓ Full |
| E2-006 (Disconnect) | TCI-001, TCI-009-010, TCI-043-045 | ✓ Full |
| E3-002 (1 Hz blink) | TCI-047-048, TCI-050-051, TCI-054-056, TCI-065-066, TCI-072 | ✓ Full |
| E3-003 (5 Hz blink) | TCI-049, TCI-052-054 | ✓ Full |
| E4-002 (Diag output) | TCI-057-061, TCI-083 | ✓ Full |
| E4-003 (Verbosity) | TCI-062-064 | ✓ Full |
| E5-002 (100 ms sample) | TCI-024, TCI-096-099 | ✓ Full |
| E5-003 (Missed samples) | — | ⚠ Integration |
| E6-001 (Super-loop) | — | Integration test |
| E6-002 (Static allocation) | — | Build verification |
| E6-003 (MCC integration) | — | Build verification |

### Coverage Notes

- **E5-003 (Handle missed samples):** Integration-level behavior in main loop. Add to integration test plan.
- **E6-001, E6-002, E6-003:** Architectural constraints verified by integration tests and build analysis, not unit tests.

---

## 6. Test Execution Order

Modules should be implemented and tested top-down, driven by application needs:

1. **Application Layer** (drives interface requirements)
   - M-02: app_state (pure logic, no dependencies)
   - M-01: main (integration - tested last)

2. **Service Layer** (implements application-needed behavior)
   - M-04: ble_svc (core BLE functionality)
   - M-03: measurement_svc (ADC reading)
   - M-05: led_svc (status indication)
   - M-06: diag_svc (optional diagnostics)

3. **HAL Layer** (implements service-needed abstractions)
   - M-09: hal_uart (needed by ble_svc, diag_svc)
   - M-08: hal_spi (needed by measurement_svc)
   - M-07: hal_gpio (needed by led_svc, measurement_svc)
   - M-10: hal_tick (needed by led_svc, diag_svc)

**Rationale:** Top-down TDD drives interfaces from actual usage. Higher layers define what they need; lower layers implement only what's required. This avoids speculative HAL APIs and ensures every function has a caller.

---

## 7. Change Log

| Date | Version | Author | Changes |
|------|---------|--------|---------|
| 2026-09-30 | 1.0 | Brian Tate | Initial creation |
| 2026-09-30 | 1.1 | Brian Tate | Moved SPI CS management to hal_spi; simplified hal_spi to single transfer function; fixed hal_uart_rx_byte to return bool with output param; simplified hal_tick to init/get_ms only; changed test order to top-down TDD |
| 2026-09-30 | 1.2 | Brian Tate | Renumbered all tests sequentially (TCI-001 to TCI-099); updated traceability matrix |
