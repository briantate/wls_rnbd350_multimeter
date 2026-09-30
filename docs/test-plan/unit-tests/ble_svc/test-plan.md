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
| TCI-027 | Call init | Configures UART via hal_uart | Expected |

### 2.2 ble_svc_poll

| Test ID | Scenario | Expected Result | Category |
|---------|----------|-----------------|----------|
| TCI-028 | No data available | Returns BLE_EVENT_NONE | Expected |
| TCI-029 | `%CONNECT%` received | Returns BLE_EVENT_CONNECT | Expected |
| TCI-030 | `%DISCONNECT%` received | Returns BLE_EVENT_DISCONNECT | Expected |
| TCI-031 | `%STREAM_OPEN%` received | Returns BLE_EVENT_STREAM_OPEN | Expected |
| TCI-032 | Partial message | Returns BLE_EVENT_NONE | Expected |
| TCI-033 | Fragmented `%CONNECT%` (byte-at-a-time) | Eventually returns CONNECT | Expected |
| TCI-034 | Fragmented `%DISCONNECT%` | Eventually returns DISCONNECT | Expected |
| TCI-035 | Fragmented `%STREAM_OPEN%` | Eventually returns STREAM_OPEN | Expected |
| TCI-036 | Adjacent messages `%CONNECT%%STREAM_OPEN%` | Parses both sequentially | Expected |
| TCI-037 | Garbage data before message | Ignores garbage, parses message | Error |
| TCI-038 | Buffer overflow (long garbage) | Recovers gracefully | Error |
| TCI-048 | Single byte at a time | Accumulates and parses correctly | Expected |

### 2.3 ble_svc_transmit_measurement

| Test ID | Scenario | Expected Result | Category |
|---------|----------|-----------------|----------|
| TCI-039 | Normal transmission | Formats as JSON | Expected |
| TCI-040 | JSON structure | `{"Meter":{"ohm":<value>}}` | Expected |
| TCI-041 | Message terminator | Includes `\r\n` | Expected |
| TCI-042 | Zero ohms | Formats as `{"Meter":{"ohm":0}}` | Boundary |
| TCI-043 | Max ohms (4294967295) | Formats correctly | Boundary |
| TCI-044 | Transmission | Sends via hal_uart | Expected |

### 2.4 ble_svc_discard_pending

| Test ID | Scenario | Expected Result | Category |
|---------|----------|-----------------|----------|
| TCI-045 | TX buffer has data | Clears TX buffer | Expected |
| TCI-046 | RX buffer has data | Clears RX buffer | Expected |
| TCI-047 | Parser mid-message | Resets parser state | Expected |

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
| TCI-027 | E2-002 | UART initialized for RNBD350 |
| TCI-028-038, TCI-048 | E2-003 | Status messages parsed correctly |
| TCI-031, TCI-035 | E2-004 | STREAM_OPEN detected |
| TCI-039-044 | E2-005 | JSON formatted and transmitted |
| TCI-045-047 | E2-006 | Buffers cleared on disconnect |

---

## 7. Test Declarations

See: [test-declarations.cpp](test-declarations.cpp)
