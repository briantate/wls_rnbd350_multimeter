# Test Plan: hal_uart (M-09)

**Module:** hal_uart  
**ID:** M-09  
**Layer:** HAL  
**Date:** 2026-09-30  
**Status:** Draft

---

## 1. Module Under Test

The `hal_uart` module provides UART abstraction for BLE module (SERCOM0) and diagnostic output (SERCOM2).

**Public Interface:**
```c
typedef enum {
    HAL_UART_BLE,   // SERCOM0
    HAL_UART_DIAG   // SERCOM2
} hal_uart_channel_t;

void hal_uart_init(hal_uart_channel_t ch);
bool hal_uart_tx_ready(hal_uart_channel_t ch);
void hal_uart_tx_byte(hal_uart_channel_t ch, uint8_t byte);
void hal_uart_tx_string(hal_uart_channel_t ch, const char* str);
bool hal_uart_rx_available(hal_uart_channel_t ch);
bool hal_uart_rx_byte(hal_uart_channel_t ch, uint8_t* byte);
```

---

## 2. Test Coverage Matrix

### 2.1 hal_uart_init

| Test ID | Scenario | Expected Result | Category |
|---------|----------|-----------------|----------|
| TCI-082 | Init BLE channel | Configures SERCOM0 | Expected |
| TCI-083 | Init DIAG channel | Configures SERCOM2 | Expected |

### 2.2 hal_uart_tx_ready

| Test ID | Scenario | Expected Result | Category |
|---------|----------|-----------------|----------|
| TCI-084 | TX buffer empty | Returns true | Expected |
| TCI-085 | TX buffer full | Returns false | Expected |

### 2.3 hal_uart_tx_byte / hal_uart_tx_string

| Test ID | Scenario | Expected Result | Category |
|---------|----------|-----------------|----------|
| TCI-086 | TX single byte | Byte sent to driver | Expected |
| TCI-087 | TX string | All characters sent | Expected |
| TCI-088 | TX string | Stops at null terminator | Expected |
| TCI-089 | TX empty string | No bytes sent | Boundary |

### 2.4 hal_uart_rx_available / hal_uart_rx_byte

| Test ID | Scenario | Expected Result | Category |
|---------|----------|-----------------|----------|
| TCI-090 | RX data available | rx_available returns true | Expected |
| TCI-091 | RX buffer empty | rx_available returns false | Expected |
| TCI-092 | RX byte when data available | Returns true, outputs byte | Expected |
| TCI-093 | RX byte when empty | Returns false, byte unchanged | Expected |

### 2.5 Channel Independence

| Test ID | Scenario | Expected Result | Category |
|---------|----------|-----------------|----------|
| TCI-094 | Invalid channel | No effect, no crash | Error |
| TCI-095 | Both channels active | Operate independently | Expected |

---

## 3. UART Configuration

| Channel | Peripheral | Baud Rate | Purpose |
|---------|------------|-----------|---------|
| HAL_UART_BLE | SERCOM0 | 115200 | RNBD350 BLE module |
| HAL_UART_DIAG | SERCOM2 | 115200 | Diagnostic output |

---

## 4. Dependencies

| Dependency | Mock Strategy |
|------------|---------------|
| MCC UART driver | Mock on host; real on target |

---

## 5. Traceability

| Test ID | Enabler Story | Acceptance Criteria |
|---------|---------------|---------------------|
| TCI-082, TCI-095 | E2-002 | BLE UART initialized |
| TCI-083, TCI-095 | E4-002 | Diagnostic UART initialized |
| TCI-084-089 | E2-005 | TX functions work |
| TCI-090-093 | E2-003 | RX functions work |

---

## 6. Test Declarations

See: [test-declarations.cpp](test-declarations.cpp)
