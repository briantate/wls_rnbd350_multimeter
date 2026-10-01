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
| E2-002 | Initialize RNBD350 communication | ble_svc, hal_uart | — | ⏳ Not Started |
| E2-003 | Parse RNBD350 status messages | ble_svc | — | ⏳ Not Started |
| E2-005 | Format and transmit JSON | ble_svc | — | ⏳ Not Started |
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

### 3.3 ble_svc (M-04) — ⏳ Not Started

| Test ID | Test Name | Function Under Test | User Story |
|---------|-----------|---------------------|------------|
| — | — | — | E2-002, E2-003, E2-005 |

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
| Modules | 9 | 2 | 7 |
| Test Cases | 121 | 24 | 97 |
| User Stories (Enabler) | 15 | 5 | 10 |

### Stories with Test Coverage

| Story | Status |
|-------|--------|
| E1-002 | ✅ Covered by TCI-014, TCI-016, TCI-020-024 |
| E1-003 | ✅ Covered by TCI-015, TCI-017-019 |
| E1-004 | ✅ Covered by TCI-013 |
| E2-004 | ✅ Covered by TCI-002 through TCI-008, TCI-011-012 |
| E2-006 | ✅ Covered by TCI-001, TCI-009-010 |

### Stories Awaiting Test Coverage
- E2-002, E2-003, E2-005 (ble_svc)
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
| firmware/src/services/ble_svc.c | Stub only — awaiting TDD |
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

---

## 6. Verification Statement

This traceability matrix will be updated as each module completes TDD implementation. Full traceability verification will occur at Gate G7 (Integration Review).

**Last Verified:** 2026-10-01  
**Verified By:** (pending G7 review)
