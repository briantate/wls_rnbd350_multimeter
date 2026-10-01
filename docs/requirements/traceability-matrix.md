# Requirements Traceability Matrix

**Project:** Bluetooth Ohmmeter Firmware  
**Last Updated:** 2026-10-01 (updated after measurement_svc)  
**Status:** In Progress

---

## 1. Overview

This document provides bidirectional traceability between:
- **User Stories** → **Test Cases** → **Source Code**
- **Source Code** → **Test Cases** → **User Stories**

Per the SDLC manual (Section 10), traceability ensures every requirement is tested and every piece of code exists for a reason.

---

## 2. User Stories → Test Cases

| Story ID | Story Title | Module(s) | Test IDs | Test Status |
|----------|-------------|-----------|----------|-------------|
| E2-004 | Detect STREAM_OPEN event | app_state | TCI-002, TCI-003-008, TCI-011-012 | ✅ 10/10 Pass |
| E2-006 | Handle BLE disconnect | app_state | TCI-001, TCI-009-010 | ✅ 3/3 Pass |
| E1-002 | Read resistance from MCP3204 | measurement_svc, hal_spi | TCI-014, TCI-020-024 | ✅ 7/7 Pass |
| E1-003 | Convert ADC counts to ohms | measurement_svc | TCI-015, TCI-017-019 | ✅ 4/4 Pass |
| E2-002 | Initialize RNBD350 communication | ble_svc, hal_uart | TCI-025 | ✅ 1/1 Pass |
| E2-003 | Parse RNBD350 status messages | ble_svc | TCI-026-036, TCI-046 | ✅ 12/12 Pass |
| E2-005 | Format and transmit JSON | ble_svc | TCI-037-042 | ✅ 6/6 Pass |
| E3-002 | LED toggle 1 Hz (disconnected) | led_svc, hal_gpio | — | ⏳ Not Started |
| E3-003 | LED toggle 5 Hz (connected) | led_svc, hal_gpio | — | ⏳ Not Started |
| E4-002 | Diagnostic output on SERCOM2 | diag_svc, hal_uart | — | ⏳ Not Started |
| E5-002 | Sample at 100 ms interval | hal_tick | — | ⏳ Not Started |

---

## 3. Modules → Test Cases → User Stories

### 3.1 app_state (M-02) — ✅ Complete

| Test ID | Test Name | Function Under Test | User Story |
|---------|-----------|---------------------|------------|
| TCI-001 | Init_SetsDisconnected | app_state_init | E2-006 |
| TCI-002 | Get_ReturnsCurrentState | app_state_get | E2-004 |
| TCI-003 | OnConnect_WhenDisconnected_TransitionsToConnected | app_state_on_connect | E2-004 |
| TCI-004 | OnConnect_WhenConnected_RemainsConnected | app_state_on_connect | E2-004 |
| TCI-005 | OnConnect_WhenStreaming_RemainsStreaming | app_state_on_connect | E2-004 |
| TCI-006 | OnStreamOpen_WhenConnected_TransitionsToStreaming | app_state_on_stream_open | E2-004 |
| TCI-007 | OnStreamOpen_WhenDisconnected_RemainsDisconnected | app_state_on_stream_open | E2-004 |
| TCI-008 | OnStreamOpen_WhenStreaming_RemainsStreaming | app_state_on_stream_open | E2-004 |
| TCI-009 | OnDisconnect_WhenConnected_TransitionsToDisconnected | app_state_on_disconnect | E2-006 |
| TCI-010 | OnDisconnect_WhenStreaming_TransitionsToDisconnected | app_state_on_disconnect | E2-006 |
| TCI-011 | IsStreaming_WhenStreaming_ReturnsTrue | app_state_is_streaming | E2-004 |
| TCI-012 | IsStreaming_WhenNotStreaming_ReturnsFalse | app_state_is_streaming | E2-004 |

**Source File:** `firmware/src/app/app_state.c`  
**Test File:** `firmware/tests/app_state/test_app_state.cpp`  
**Coverage:** 12/12 tests passing (100%)

### 3.2 measurement_svc (M-03) — ✅ Complete

| Test ID | Test Name | Function Under Test | User Story |
|---------|-----------|---------------------|------------|
| TCI-013 | Init_ConfiguresRangeGpios | measurement_svc_init | E1-004 |
| TCI-014 | Sample_ReadsAdcViaHal | measurement_svc_sample | E1-002 |
| TCI-015 | Sample_ConvertsCountsToOhms | measurement_svc_sample | E1-003 |
| TCI-016 | Sample_ReturnsValidMeasurement | measurement_svc_sample | E1-002 |
| TCI-017 | Sample_ZeroCounts_ReturnsZeroOhms | measurement_svc_sample | E1-003 |
| TCI-018 | Sample_MaxCounts_ReturnsMaxOhms | measurement_svc_sample | E1-003 |
| TCI-019 | Sample_MidCounts_ReturnsCorrectOhms | measurement_svc_sample | E1-003 |
| TCI-020 | Sample_SendsCorrectSpiCommand | measurement_svc_sample | E1-002 |
| TCI-021 | Sample_Channel2Selected | measurement_svc_sample | E1-002 |
| TCI-022 | Sample_SpiError_ReturnsInvalid | measurement_svc_sample | E1-002 |
| TCI-023 | Sample_MultipleConsecutive_WorksCorrectly | measurement_svc_sample | E1-002 |
| TCI-024 | Sample_TimingWithinBudget | measurement_svc_sample | E5-002 |

**Source File:** `firmware/src/services/measurement_svc.c`  
**Test File:** `firmware/tests/measurement_svc/test_measurement_svc.cpp`  
**Coverage:** 12/12 tests passing (100%)

### 3.3 ble_svc (M-04) — ✅ Complete

| Test ID | Test Name | Function Under Test | User Story |
|---------|-----------|---------------------|------------|
| TCI-025 | Init_ConfiguresUart | ble_svc_init | E2-002 |
| TCI-026 | Poll_NoData_ReturnsNone | ble_svc_poll | E2-003 |
| TCI-027 | Poll_ConnectMessage_ReturnsConnect | ble_svc_poll | E2-003 |
| TCI-028 | Poll_DisconnectMessage_ReturnsDisconnect | ble_svc_poll | E2-003 |
| TCI-029 | Poll_StreamOpenMessage_ReturnsStreamOpen | ble_svc_poll | E2-004 |
| TCI-030 | Poll_PartialMessage_ReturnsNone | ble_svc_poll | E2-003 |
| TCI-031 | Poll_FragmentedConnect_EventuallyReturnsConnect | ble_svc_poll | E2-003 |
| TCI-032 | Poll_FragmentedDisconnect_EventuallyReturnsDisconnect | ble_svc_poll | E2-003 |
| TCI-033 | Poll_FragmentedStreamOpen_EventuallyReturnsStreamOpen | ble_svc_poll | E2-004 |
| TCI-034 | Poll_AdjacentMessages_ParsesBoth | ble_svc_poll | E2-003 |
| TCI-035 | Poll_GarbageData_IgnoresAndContinues | ble_svc_poll | E2-003 |
| TCI-036 | Poll_BufferOverflow_RecoversGracefully | ble_svc_poll | E2-003 |
| TCI-037 | TransmitMeasurement_FormatsJson | ble_svc_transmit_measurement | E2-005 |
| TCI-038 | TransmitMeasurement_CorrectJsonStructure | ble_svc_transmit_measurement | E2-005 |
| TCI-039 | TransmitMeasurement_IncludesCrLf | ble_svc_transmit_measurement | E2-005 |
| TCI-040 | TransmitMeasurement_ZeroOhms_FormatsCorrectly | ble_svc_transmit_measurement | E2-005 |
| TCI-041 | TransmitMeasurement_MaxOhms_FormatsCorrectly | ble_svc_transmit_measurement | E2-005 |
| TCI-042 | TransmitMeasurement_SendsViaUart | ble_svc_transmit_measurement | E2-005 |
| TCI-043 | DiscardPending_ClearsTxBuffer | ble_svc_discard_pending | E2-006 |
| TCI-044 | DiscardPending_ClearsRxBuffer | ble_svc_discard_pending | E2-006 |
| TCI-045 | DiscardPending_ResetsParserState | ble_svc_discard_pending | E2-006 |
| TCI-046 | Poll_ByteAtATime_AccumulatesCorrectly | ble_svc_poll | E2-003 |

**Source File:** `firmware/src/services/ble_svc.c`  
**Test File:** `firmware/tests/ble_svc/test_ble_svc.cpp`  
**Coverage:** 22/22 tests passing (100%)

### 3.4 led_svc (M-05) — ⏳ Not Started

| Test ID | Test Name | Function Under Test | User Story |
|---------|-----------|---------------------|------------|
| — | — | — | E3-002, E3-003 |

### 3.5 diag_svc (M-06) — ⏳ Not Started

| Test ID | Test Name | Function Under Test | User Story |
|---------|-----------|---------------------|------------|
| — | — | — | E4-002, E4-003 |

### 3.6 hal_gpio (M-07) — ⏳ Not Started

| Test ID | Test Name | Function Under Test | User Story |
|---------|-----------|---------------------|------------|
| — | — | — | E3-002, E3-003 |

### 3.7 hal_spi (M-08) — ⏳ Not Started

| Test ID | Test Name | Function Under Test | User Story |
|---------|-----------|---------------------|------------|
| — | — | — | E1-002 |

### 3.8 hal_uart (M-09) — ⏳ Not Started

| Test ID | Test Name | Function Under Test | User Story |
|---------|-----------|---------------------|------------|
| — | — | — | E2-002, E4-002 |

### 3.9 hal_tick (M-10) — ⏳ Not Started

| Test ID | Test Name | Function Under Test | User Story |
|---------|-----------|---------------------|------------|
| — | — | — | E5-002 |

---

## 4. Coverage Summary

| Category | Total | Implemented | Remaining |
|----------|-------|-------------|-----------|
| Modules | 9 | 3 | 6 |
| Test Cases | 99 | 46 | 53 |
| User Stories (Enabler) | 15 | 8 | 7 |

### Stories with Test Coverage

| Story | Status |
|-------|--------|
| E1-002 | ✅ Covered by TCI-014, TCI-016, TCI-020-024 |
| E1-003 | ✅ Covered by TCI-015, TCI-017-019 |
| E1-004 | ✅ Covered by TCI-013 |
| E2-002 | ✅ Covered by TCI-025 |
| E2-003 | ✅ Covered by TCI-026-036, TCI-046 |
| E2-004 | ✅ Covered by TCI-002-008, TCI-011-012, TCI-029, TCI-033 |
| E2-005 | ✅ Covered by TCI-037-042 |
| E2-006 | ✅ Covered by TCI-001, TCI-009-010, TCI-043-045 |

### Stories Awaiting Test Coverage
- E3-002, E3-003 (led_svc)
- E4-002, E4-003 (diag_svc)
- E5-002, E5-003 (hal_tick)
- E6-001, E6-002, E6-003 (technical/NFR — verified by architecture)

---

## 5. Orphan Analysis

### Orphan Tests (tests without requirements)
None identified.

### Orphan Code (code without tests)
| File | Status |
|------|--------|
| firmware/src/services/led_svc.c | Stub only — awaiting TDD |
| firmware/src/services/diag_svc.c | Stub only — awaiting TDD |
| firmware/src/hal/hal_gpio.c | Stub only — awaiting TDD |
| firmware/src/hal/hal_spi.c | Stub only — awaiting TDD |
| firmware/src/hal/hal_uart.c | Stub only — awaiting TDD |
| firmware/src/hal/hal_tick.c | Stub only — awaiting TDD |

### Completed Modules (no longer orphans)
| File | Tests | Date |
|------|-------|------|
| firmware/src/app/app_state.c | 12/12 | 2026-10-01 |
| firmware/src/services/measurement_svc.c | 12/12 | 2026-10-01 |
| firmware/src/services/ble_svc.c | 22/22 | 2026-10-01 |

---

## 6. Verification Statement

This traceability matrix will be updated as each module completes TDD implementation. Full traceability verification will occur at Gate G7 (Integration Review).

**Last Verified:** 2026-10-01  
**Verified By:** (pending G7 review)
