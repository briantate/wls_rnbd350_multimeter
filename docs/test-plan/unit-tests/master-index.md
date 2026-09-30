# Master Test Index

**Project:** Bluetooth Ohmmeter Firmware  
**Version:** 1.0  
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
| hal_tick | M-10 | 8 | 100% branch | Planned |
| **Total** | | **103** | | |

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
| TCI-027 | BleSvc_Init_ConfiguresUart | E2-002 |
| TCI-028 | BleSvc_Poll_NoData_ReturnsNone | E2-003 |
| TCI-029 | BleSvc_Poll_ConnectMessage_ReturnsConnect | E2-003 |
| TCI-030 | BleSvc_Poll_DisconnectMessage_ReturnsDisconnect | E2-003 |
| TCI-031 | BleSvc_Poll_StreamOpenMessage_ReturnsStreamOpen | E2-004 |
| TCI-032 | BleSvc_Poll_PartialMessage_ReturnsNone | E2-003 |
| TCI-033 | BleSvc_Poll_FragmentedConnect_EventuallyReturnsConnect | E2-003 |
| TCI-034 | BleSvc_Poll_FragmentedDisconnect_EventuallyReturnsDisconnect | E2-003 |
| TCI-035 | BleSvc_Poll_FragmentedStreamOpen_EventuallyReturnsStreamOpen | E2-004 |
| TCI-036 | BleSvc_Poll_AdjacentMessages_ParsesBoth | E2-003 |
| TCI-037 | BleSvc_Poll_GarbageData_IgnoresAndContinues | E2-003 |
| TCI-038 | BleSvc_Poll_BufferOverflow_RecoversGracefully | E2-003 |
| TCI-039 | BleSvc_TransmitMeasurement_FormatsJson | E2-005 |
| TCI-040 | BleSvc_TransmitMeasurement_CorrectJsonStructure | E2-005 |
| TCI-041 | BleSvc_TransmitMeasurement_IncludesCrLf | E2-005 |
| TCI-042 | BleSvc_TransmitMeasurement_ZeroOhms_FormatsCorrectly | E2-005 |
| TCI-043 | BleSvc_TransmitMeasurement_MaxOhms_FormatsCorrectly | E2-005 |
| TCI-044 | BleSvc_TransmitMeasurement_SendsViaUart | E2-005 |
| TCI-045 | BleSvc_DiscardPending_ClearsTxBuffer | E2-006 |
| TCI-046 | BleSvc_DiscardPending_ClearsRxBuffer | E2-006 |
| TCI-047 | BleSvc_DiscardPending_ResetsParserState | E2-006 |
| TCI-048 | BleSvc_Poll_ByteAtATime_AccumulatesCorrectly | E2-003 |

**Test Plan:** [ble_svc/test-plan.md](ble_svc/test-plan.md)

---

### 4.4 M-05: led_svc

| Test ID | Test Name | Traces To |
|---------|-----------|-----------|
| TCI-049 | LedSvc_Init_TurnsLedOff | E3-002 |
| TCI-050 | LedSvc_Update_WhenDisconnected_TogglesAt1000ms | E3-002 |
| TCI-051 | LedSvc_Update_WhenStreaming_TogglesAt200ms | E3-003 |
| TCI-052 | LedSvc_Update_Before1000ms_NoToggle | E3-002 |
| TCI-053 | LedSvc_Update_At1000ms_Toggles | E3-002 |
| TCI-054 | LedSvc_Update_Before200ms_NoToggle | E3-003 |
| TCI-055 | LedSvc_Update_At200ms_Toggles | E3-003 |
| TCI-056 | LedSvc_Update_StateChangesMidCycle_AdjustsRate | E3-002, E3-003 |
| TCI-057 | LedSvc_Update_AccumulatesElapsedTime | E3-002 |
| TCI-058 | LedSvc_Update_ResetsAfterToggle | E3-002 |

**Test Plan:** [led_svc/test-plan.md](led_svc/test-plan.md)

---

### 4.5 M-06: diag_svc

| Test ID | Test Name | Traces To |
|---------|-----------|-----------|
| TCI-059 | DiagSvc_Init_ConfiguresUart | E4-002 |
| TCI-060 | DiagSvc_Log_FormatsTimestamp | E4-002 |
| TCI-061 | DiagSvc_Log_FormatsLevel | E4-002 |
| TCI-062 | DiagSvc_Log_FormatsSubsystem | E4-002 |
| TCI-063 | DiagSvc_Log_FormatsMessage | E4-002 |
| TCI-064 | DiagSvc_Log_BelowVerbosity_NoOutput | E4-003 |
| TCI-065 | DiagSvc_Log_AtVerbosity_Outputs | E4-003 |
| TCI-066 | DiagSvc_Log_AboveVerbosity_Outputs | E4-003 |

**Test Plan:** [diag_svc/test-plan.md](diag_svc/test-plan.md)

---

### 4.6 M-07: hal_gpio

| Test ID | Test Name | Traces To |
|---------|-----------|-----------|
| TCI-067 | HalGpio_Init_ConfiguresPins | E1-004, E3-002 |
| TCI-068 | HalGpio_Write_Led_SetsState | E3-002 |
| TCI-069 | HalGpio_Write_ResA_SetsState | E1-004 |
| TCI-070 | HalGpio_Write_ResB_SetsState | E1-004 |
| TCI-071 | HalGpio_Write_ResC_SetsState | E1-004 |
| TCI-072 | HalGpio_Write_SpiCs_SetsState | E1-002 |
| TCI-073 | HalGpio_Read_ReturnsCurrentState | E1-004 |
| TCI-074 | HalGpio_Toggle_InvertsState | E3-002 |
| TCI-075 | HalGpio_Write_InvalidPin_NoEffect | - |
| TCI-076 | HalGpio_Read_InvalidPin_ReturnsFalse | - |

**Test Plan:** [hal_gpio/test-plan.md](hal_gpio/test-plan.md)

---

### 4.7 M-08: hal_spi

| Test ID | Test Name | Traces To |
|---------|-----------|-----------|
| TCI-077 | HalSpi_Init_ConfiguresPeripheral | E1-002 |
| TCI-078 | HalSpi_Transfer_SendsAllBytes | E1-002 |
| TCI-079 | HalSpi_Transfer_ReceivesAllBytes | E1-002 |
| TCI-080 | HalSpi_Transfer_ZeroLength_NoOp | E1-002 |
| TCI-081 | HalSpi_Transfer_NullRx_SendsOnly | E1-002 |
| TCI-082 | HalSpi_Transfer_AssertsCs | E1-002 |
| TCI-083 | HalSpi_Transfer_DeassertsCs | E1-002 |

**Test Plan:** [hal_spi/test-plan.md](hal_spi/test-plan.md)

---

### 4.8 M-09: hal_uart

| Test ID | Test Name | Traces To |
|---------|-----------|-----------|
| TCI-085 | HalUart_Init_Ble_ConfiguresSercom0 | E2-002 |
| TCI-086 | HalUart_Init_Diag_ConfiguresSercom2 | E4-002 |
| TCI-087 | HalUart_TxReady_WhenReady_ReturnsTrue | E2-005 |
| TCI-088 | HalUart_TxReady_WhenBusy_ReturnsFalse | E2-005 |
| TCI-089 | HalUart_TxByte_SendsByte | E2-005 |
| TCI-090 | HalUart_TxString_SendsAllChars | E2-005 |
| TCI-091 | HalUart_TxString_NullTerminated | E2-005 |
| TCI-092 | HalUart_TxString_EmptyString_NoOutput | E2-005 |
| TCI-093 | HalUart_RxAvailable_WhenData_ReturnsTrue | E2-003 |
| TCI-094 | HalUart_RxAvailable_WhenEmpty_ReturnsFalse | E2-003 |
| TCI-095 | HalUart_RxByte_ReturnsByte | E2-003 |
| TCI-096 | HalUart_RxByte_WhenEmpty_ReturnsZero | E2-003 |
| TCI-097 | HalUart_InvalidChannel_NoEffect | - |
| TCI-098 | HalUart_BothChannels_Independent | E2-002, E4-002 |

**Test Plan:** [hal_uart/test-plan.md](hal_uart/test-plan.md)

---

### 4.9 M-10: hal_tick

| Test ID | Test Name | Traces To |
|---------|-----------|-----------|
| TCI-099 | HalTick_Init_StartsTimer | E5-002 |
| TCI-100 | HalTick_GetMs_ReturnsMilliseconds | E5-002 |
| TCI-101 | HalTick_GetMs_IncrementsOverTime | E5-002 |
| TCI-102 | HalTick_CheckFlag_AfterTick_ReturnsTrue | E5-002 |
| TCI-103 | HalTick_CheckFlag_Clears_Flag | E5-002 |
| TCI-104 | HalTick_CheckFlag_NoTick_ReturnsFalse | E5-002 |
| TCI-105 | HalTick_ClearFlag_ClearsFlag | E5-002 |
| TCI-106 | HalTick_AdvanceMs_Mock_AdvancesTime | E5-002 |

**Test Plan:** [hal_tick/test-plan.md](hal_tick/test-plan.md)

---

## 5. Traceability Matrix

| Enabler Story | Test IDs |
|---------------|----------|
| E1-002 (Read ADC) | TCI-014, TCI-020-026, TCI-077-084 |
| E1-003 (Convert to ohms) | TCI-015, TCI-017-019 |
| E1-004 (Configure range) | TCI-013, TCI-067, TCI-069-071, TCI-073 |
| E2-002 (UART init) | TCI-027, TCI-085-086, TCI-098 |
| E2-003 (Status parsing) | TCI-028-037, TCI-048, TCI-093-096 |
| E2-004 (STREAM_OPEN) | TCI-002, TCI-003-008, TCI-011-012, TCI-031, TCI-035 |
| E2-005 (JSON TX) | TCI-039-044, TCI-087-092 |
| E2-006 (Disconnect) | TCI-001, TCI-009-010, TCI-045-047 |
| E3-002 (1 Hz blink) | TCI-049-053, TCI-057-058, TCI-068, TCI-074 |
| E3-003 (5 Hz blink) | TCI-051, TCI-054-056 |
| E4-002 (Diag output) | TCI-059-063, TCI-086 |
| E4-003 (Verbosity) | TCI-064-066 |
| E5-002 (100 ms sample) | TCI-026, TCI-099-106 |
| E6-001 (Super-loop) | Integration tests (not unit) |

---

## 6. Test Execution Order

Modules should be implemented and tested in dependency order (bottom-up):

1. **HAL Layer** (no dependencies)
   - M-10: hal_tick
   - M-07: hal_gpio
   - M-08: hal_spi
   - M-09: hal_uart

2. **Service Layer** (depends on HAL)
   - M-03: measurement_svc
   - M-06: diag_svc
   - M-04: ble_svc
   - M-05: led_svc

3. **Application Layer** (depends on Services)
   - M-02: app_state
   - M-01: main (integration)

---

## 7. Change Log

| Date | Version | Author | Changes |
|------|---------|--------|---------|
| 2026-09-30 | 1.0 | Claude | Initial creation |
