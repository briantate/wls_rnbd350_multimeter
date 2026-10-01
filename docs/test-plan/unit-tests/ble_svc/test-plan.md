# Test Plan: ble_svc (M-04)

**Module:** ble_svc  
**ID:** M-04  
**Layer:** Service  
**Date:** 2026-09-30  
**Status:** Draft

---

## 1. Module Under Test

The `ble_svc` module handles RNBD350 BLE module communication: parsing incoming status messages and transmitting JSON-formatted measurement data.

**Public Interface:**
```c
typedef enum {
    BLE_EVENT_NONE,
    BLE_EVENT_CONNECT,
    BLE_EVENT_DISCONNECT,
    BLE_EVENT_STREAM_OPEN
} ble_event_t;

void        ble_svc_init(void);
ble_event_t ble_svc_poll(void);
void        ble_svc_transmit_measurement(uint32_t ohms);
void        ble_svc_discard_pending(void);
```

---

## 2. Test Coverage Matrix

### 2.1 ble_svc_init

| Test ID | Scenario | Expected Result | Category |
|---------|----------|-----------------|----------|
| TCI-025 | Call init | Configures UART via hal_uart | Expected |

### 2.2 ble_svc_poll

| Test ID | Scenario | Expected Result | Category |
|---------|----------|-----------------|----------|
| TCI-026 | No data available | Returns BLE_EVENT_NONE | Expected |
| TCI-027 | `%CONNECT%` received | Returns BLE_EVENT_CONNECT | Expected |
| TCI-028 | `%DISCONNECT%` received | Returns BLE_EVENT_DISCONNECT | Expected |
| TCI-029 | `%STREAM_OPEN%` received | Returns BLE_EVENT_STREAM_OPEN | Expected |
| TCI-030 | Partial message | Returns BLE_EVENT_NONE | Expected |
| TCI-031 | Fragmented `%CONNECT%` (byte-at-a-time) | Eventually returns CONNECT | Expected |
| TCI-032 | Fragmented `%DISCONNECT%` | Eventually returns DISCONNECT | Expected |
| TCI-033 | Fragmented `%STREAM_OPEN%` | Eventually returns STREAM_OPEN | Expected |
| TCI-034 | Adjacent messages `%CONNECT%%STREAM_OPEN%` | Parses both sequentially | Expected |
| TCI-035 | Garbage data before message | Ignores garbage, parses message | Error |
| TCI-036 | Buffer overflow (long garbage) | Recovers gracefully | Error |
| TCI-046 | Single byte at a time | Accumulates and parses correctly | Expected |

### 2.3 ble_svc_transmit_measurement

| Test ID | Scenario | Expected Result | Category |
|---------|----------|-----------------|----------|
| TCI-037 | Normal transmission | Formats as JSON | Expected |
| TCI-038 | JSON structure | `{"Meter":{"ohm":<value>}}` | Expected |
| TCI-039 | Message terminator | Includes `\r\n` | Expected |
| TCI-040 | Zero ohms | Formats as `{"Meter":{"ohm":0}}` | Boundary |
| TCI-041 | Max ohms (4294967295) | Formats correctly | Boundary |
| TCI-042 | Transmission | Sends via hal_uart | Expected |

### 2.4 ble_svc_discard_pending

| Test ID | Scenario | Expected Result | Category |
|---------|----------|-----------------|----------|
| TCI-043 | TX buffer has data | Clears TX buffer | Expected |
| TCI-044 | RX buffer has data | Clears RX buffer | Expected |
| TCI-045 | Parser mid-message | Resets parser state | Expected |

---

## 3. RNBD350 Status Messages

| Message | Meaning |
|---------|---------|
| `%CONNECT%` | BLE central connected |
| `%DISCONNECT%` | BLE central disconnected |
| `%STREAM_OPEN%` | Transparent UART mode active |

**Parser State Machine:**
```
IDLE ──'%'──► PARSING ──message complete──► EVENT_READY ──poll()──► IDLE
                │
                └── '%' without match ──► IDLE (reset)
```

---

## 4. JSON Format

```json
{"Meter":{"ohm":12345}}\r\n
```

- No spaces (compact)
- Integer value only (no decimal)
- Terminated with CR+LF
- Max length: ~32 characters for max uint32

---

## 5. Dependencies

| Dependency | Mock Strategy |
|------------|---------------|
| hal_uart (M-09) | Mock: inject RX bytes, capture TX bytes |

---

## 6. Traceability

| Test ID | Enabler Story | Acceptance Criteria |
|---------|---------------|---------------------|
| TCI-025 | E2-002 | UART initialized for RNBD350 |
| TCI-026-036, TCI-046 | E2-003 | Status messages parsed correctly |
| TCI-029, TCI-033 | E2-004 | STREAM_OPEN detected |
| TCI-037-042 | E2-005 | JSON formatted and transmitted |
| TCI-043-045 | E2-006 | Buffers cleared on disconnect |

---

## 7. Test Declarations

See: [test-declarations.cpp](test-declarations.cpp)
