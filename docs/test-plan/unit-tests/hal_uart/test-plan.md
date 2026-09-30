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

void    hal_uart_init(hal_uart_channel_t ch);
bool    hal_uart_tx_ready(hal_uart_channel_t ch);
void    hal_uart_tx_byte(hal_uart_channel_t ch, uint8_t byte);
void    hal_uart_tx_string(hal_uart_channel_t ch, const char* str);
bool    hal_uart_rx_available(hal_uart_channel_t ch);
uint8_t hal_uart_rx_byte(hal_uart_channel_t ch);
```

---

## 2. Test Coverage Matrix

### 2.1 hal_uart_init

| Test ID | Scenario | Expected Result | Category |
|---------|----------|-----------------|----------|
| TCI-085 | Init BLE channel | Configures SERCOM0 | Expected |
| TCI-086 | Init DIAG channel | Configures SERCOM2 | Expected |

### 2.2 hal_uart_tx_ready

| Test ID | Scenario | Expected Result | Category |
|---------|----------|-----------------|----------|
| TCI-087 | TX buffer empty | Returns true | Expected |
| TCI-088 | TX buffer full | Returns false | Expected |

### 2.3 hal_uart_tx_byte / hal_uart_tx_string

| Test ID | Scenario | Expected Result | Category |
|---------|----------|-----------------|----------|
| TCI-089 | TX single byte | Byte sent to driver | Expected |
| TCI-090 | TX string | All characters sent | Expected |
| TCI-091 | TX string | Stops at null terminator | Expected |
| TCI-092 | TX empty string | No bytes sent | Boundary |

### 2.4 hal_uart_rx_available / hal_uart_rx_byte

| Test ID | Scenario | Expected Result | Category |
|---------|----------|-----------------|----------|
| TCI-093 | RX data available | Returns true | Expected |
| TCI-094 | RX buffer empty | Returns false | Expected |
| TCI-095 | RX byte | Returns received byte | Expected |
| TCI-096 | RX when empty | Returns 0 | Boundary |

### 2.5 Channel Independence

| Test ID | Scenario | Expected Result | Category |
|---------|----------|-----------------|----------|
| TCI-097 | Invalid channel | No effect, no crash | Error |
| TCI-098 | Both channels active | Operate independently | Expected |

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
| TCI-085, TCI-098 | E2-002 | BLE UART initialized |
| TCI-086, TCI-098 | E4-002 | Diagnostic UART initialized |
| TCI-087-092 | E2-005 | TX functions work |
| TCI-093-096 | E2-003 | RX functions work |

---

## 6. Test Declarations

See: [test-declarations.cpp](test-declarations.cpp)
