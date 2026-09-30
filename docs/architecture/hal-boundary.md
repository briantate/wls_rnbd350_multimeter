# HAL Boundary Definition

**Project:** Bluetooth Ohmmeter Firmware  
**Version:** 1.0  
**Date:** 2026-09-30  
**Status:** Draft

---

## 1. Purpose

This document defines the Hardware Abstraction Layer (HAL) boundary for the Bluetooth Ohmmeter firmware. The HAL provides a portable interface between application/service code and hardware-specific drivers, enabling:

1. **Host-side unit testing** with mock implementations
2. **Portability** to alternative MCU platforms
3. **Isolation** from vendor-specific (MCC) driver changes

---

## 2. Boundary Principle

```
+---------------------------------------------------------+
|  ABOVE THE HAL (portable, testable)                     |
|  - Application layer (main, app_state)                  |
|  - Service layer (measurement_svc, ble_svc, led_svc,    |
|    diag_svc)                                            |
|  - Pure C with no hardware dependencies                 |
|  - Compiles for host (x86/x64) and target (ARM)         |
+---------------------------------------------------------+
                           |
                           | HAL API (hal_*.h)
                           v
+---------------------------------------------------------+
|  HAL INTERFACE (portable function signatures)           |
|  - hal_gpio.h, hal_spi.h, hal_uart.h, hal_tick.h        |
|  - Synchronous, blocking where timing budget allows     |
|  - No MCC types in public interface                     |
|  - Fixed-width integer types (stdint.h)                 |
+---------------------------------------------------------+
                           |
                           | Implementation selection (compile-time)
                           v
+---------------------------------------------------------+
|  BELOW THE HAL (target-specific, not unit-tested)       |
|  - HAL implementation (src/hal/*.c)                     |
|  - MCC-generated drivers (unmodified)                   |
|  - Startup code, vector table, linker scripts           |
|  - Direct register access                               |
+---------------------------------------------------------+
```

---

## 3. HAL Design Rules

### 3.1 Interface Rules

| Rule | Description |
|------|-------------|
| R-01 | HAL headers use only standard C types (`stdint.h`, `stdbool.h`, `stddef.h`) |
| R-02 | No MCC types, macros, or includes in HAL headers |
| R-03 | All functions are synchronous (no callbacks across HAL boundary) |
| R-04 | Error handling via return codes or out-parameters, not exceptions |
| R-05 | No global state visible above HAL; use opaque handles if needed |
| R-06 | ISRs handled internally; communicate via flags or ring buffers |

### 3.2 Implementation Rules

| Rule | Description |
|------|-------------|
| R-07 | Target HAL implementation calls MCC drivers |
| R-08 | Mock HAL implementation provides test doubles |
| R-09 | HAL implementations are selected at link time (not runtime) |
| R-10 | MCC driver files are never modified |

---

## 4. Peripheral Coverage

### 4.1 Peripherals Requiring HAL Abstraction

| Peripheral | HAL Module | Physical Resource | Usage |
|------------|------------|-------------------|-------|
| GPIO       | hal_gpio   | Multiple pins     | LED, range selection, SPI CS |
| SPI        | hal_spi    | SERCOM5           | MCP3204 ADC communication |
| UART       | hal_uart   | SERCOM0, SERCOM2  | RNBD350 BLE, diagnostics |
| Timer      | hal_tick   | TC0               | 1 ms system tick |

### 4.2 Peripherals NOT Requiring HAL (Out of Scope)

| Peripheral | Reason |
|------------|--------|
| Internal ADC | Not used (external MCP3204 via SPI) |
| DMA | Not used in this design |
| I2C | Not used |
| PWM | Not used |
| Sleep modes | Not used (continuous operation) |
| Watchdog | Optional, can be added later |
| NVIC | Configured by MCC startup; not abstracted |

---

## 5. HAL API Contracts

### 5.1 hal_gpio.h

**Purpose:** Abstract GPIO pin operations for LED, range selection, and SPI chip select.

```c
#ifndef HAL_GPIO_H
#define HAL_GPIO_H

#include <stdbool.h>

/**
 * @brief Logical pin identifiers (portable across platforms)
 */
typedef enum {
    HAL_PIN_LED,        /**< Status LED output */
    HAL_PIN_RES_A,      /**< Range selection bit A (CD4028B) */
    HAL_PIN_RES_B,      /**< Range selection bit B (CD4028B) */
    HAL_PIN_RES_C,      /**< Range selection bit C (CD4028B) */
    HAL_PIN_SPI_CS,     /**< MCP3204 chip select (active low) */
    HAL_PIN_COUNT       /**< Number of pins (for iteration/arrays) */
} hal_pin_t;

/**
 * @brief Initialize all GPIO pins to default states
 * @pre   System clocks initialized
 * @post  All pins configured as outputs with defined initial states
 */
void hal_gpio_init(void);

/**
 * @brief Write a logic level to a pin
 * @param pin   Pin identifier
 * @param state true = high, false = low
 */
void hal_gpio_write(hal_pin_t pin, bool state);

/**
 * @brief Read the current output state of a pin
 * @param pin   Pin identifier
 * @return      Current logic level (true = high)
 */
bool hal_gpio_read(hal_pin_t pin);

/**
 * @brief Toggle a pin's output state
 * @param pin   Pin identifier
 */
void hal_gpio_toggle(hal_pin_t pin);

#endif /* HAL_GPIO_H */
```

**Pin Mapping (Target Implementation):**

| Logical Pin   | Physical Pin | MCC Name         | Direction | Initial State |
|---------------|--------------|------------------|-----------|---------------|
| HAL_PIN_LED   | TBD          | LED0             | Output    | Low (off)     |
| HAL_PIN_RES_A | TBD          | RES_A            | Output    | Per range     |
| HAL_PIN_RES_B | TBD          | RES_B            | Output    | Per range     |
| HAL_PIN_RES_C | TBD          | RES_C            | Output    | Per range     |
| HAL_PIN_SPI_CS| TBD          | Multimeter_SPI_CS| Output    | High (deasserted) |

---

### 5.2 hal_spi.h

**Purpose:** Abstract SPI master operations for MCP3204 ADC communication.

```c
#ifndef HAL_SPI_H
#define HAL_SPI_H

#include <stdint.h>
#include <stddef.h>

/**
 * @brief Initialize SPI peripheral
 * @pre   System clocks initialized
 * @post  SPI configured: 1 MHz clock, Mode 0 (CPOL=0, CPHA=0), MSB first
 * @note  Chip select managed separately via hal_gpio
 */
void hal_spi_init(void);

/**
 * @brief Transfer a single byte (simultaneous TX and RX)
 * @param tx_byte   Byte to transmit
 * @return          Byte received during transfer
 * @note  Blocking; returns when transfer complete
 */
uint8_t hal_spi_transfer(uint8_t tx_byte);

/**
 * @brief Transfer a block of bytes
 * @param tx_buf    Pointer to transmit buffer (may be NULL for RX-only)
 * @param rx_buf    Pointer to receive buffer (may be NULL for TX-only)
 * @param len       Number of bytes to transfer
 * @note  Blocking; returns when all bytes transferred
 */
void hal_spi_transfer_block(const uint8_t* tx_buf, uint8_t* rx_buf, size_t len);

#endif /* HAL_SPI_H */
```

**SPI Configuration:**

| Parameter    | Value           | Source                    |
|--------------|-----------------|---------------------------|
| Clock rate   | 1 MHz           | MCP3204 datasheet limit   |
| Mode         | 0 (CPOL=0, CPHA=0) | MCP3204 requirement    |
| Bit order    | MSB first       | MCP3204 requirement       |
| Chip select  | Manual via GPIO | Allows multi-byte framing |
| SERCOM       | SERCOM5         | Hardware design           |

---

### 5.3 hal_uart.h

**Purpose:** Abstract UART operations for BLE module and diagnostic output.

```c
#ifndef HAL_UART_H
#define HAL_UART_H

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief UART channel identifiers
 */
typedef enum {
    HAL_UART_BLE,       /**< SERCOM0: RNBD350 BLE module */
    HAL_UART_DIAG,      /**< SERCOM2: Diagnostic output */
    HAL_UART_COUNT      /**< Number of channels */
} hal_uart_channel_t;

/**
 * @brief Initialize a UART channel
 * @param ch    Channel to initialize
 * @pre   System clocks initialized
 * @post  UART configured: 115200 baud, 8N1
 */
void hal_uart_init(hal_uart_channel_t ch);

/**
 * @brief Check if transmitter is ready for next byte
 * @param ch    Channel to check
 * @return      true if TX register empty
 */
bool hal_uart_tx_ready(hal_uart_channel_t ch);

/**
 * @brief Transmit a single byte (non-blocking if TX ready)
 * @param ch    Channel to use
 * @param byte  Byte to transmit
 * @pre   hal_uart_tx_ready(ch) == true, or will block briefly
 */
void hal_uart_tx_byte(hal_uart_channel_t ch, uint8_t byte);

/**
 * @brief Transmit a null-terminated string
 * @param ch    Channel to use
 * @param str   String to transmit
 * @note  Blocking; waits for each byte to be accepted
 */
void hal_uart_tx_string(hal_uart_channel_t ch, const char* str);

/**
 * @brief Check if received data is available
 * @param ch    Channel to check
 * @return      true if at least one byte available
 */
bool hal_uart_rx_available(hal_uart_channel_t ch);

/**
 * @brief Read a received byte
 * @param ch    Channel to read from
 * @return      Received byte
 * @pre   hal_uart_rx_available(ch) == true
 */
uint8_t hal_uart_rx_byte(hal_uart_channel_t ch);

#endif /* HAL_UART_H */
```

**UART Configuration:**

| Channel       | SERCOM   | Baud Rate | Format | Purpose                |
|---------------|----------|-----------|--------|------------------------|
| HAL_UART_BLE  | SERCOM0  | 115200    | 8N1    | RNBD350 communication  |
| HAL_UART_DIAG | SERCOM2  | 115200    | 8N1    | Debug terminal output  |

**Channel Isolation Rule:** SERCOM0 traffic is NEVER mixed with SERCOM2. BLE data only on BLE channel; diagnostics only on DIAG channel.

---

### 5.4 hal_tick.h

**Purpose:** Abstract 1 ms system tick for timing and scheduling.

```c
#ifndef HAL_TICK_H
#define HAL_TICK_H

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Initialize the system tick timer
 * @pre   System clocks initialized
 * @post  TC0 generating 1 ms periodic interrupts
 */
void hal_tick_init(void);

/**
 * @brief Get milliseconds since system start
 * @return  32-bit millisecond counter (wraps after ~49 days)
 */
uint32_t hal_tick_get_ms(void);

/**
 * @brief Check if a tick has occurred since last check
 * @return  true if at least 1 ms has elapsed since last call
 * @note    Clears the internal flag when returning true
 */
bool hal_tick_check_flag(void);

/**
 * @brief Manually clear the tick flag
 * @note  Use when resynchronizing timing
 */
void hal_tick_clear_flag(void);

/* ========== Mock-only functions ========== */
#ifdef HAL_MOCK

/**
 * @brief Advance simulated time (mock implementation only)
 * @param ms    Milliseconds to advance
 */
void hal_tick_advance_ms(uint32_t ms);

/**
 * @brief Reset tick counter to zero (mock implementation only)
 */
void hal_tick_reset(void);

#endif /* HAL_MOCK */

#endif /* HAL_TICK_H */
```

**Tick Implementation Notes:**

- TC0 configured for 1 ms period
- ISR increments counter and sets flag
- Main loop polls flag; flag cleared on read
- Counter is 32-bit, sufficient for 49+ days of operation

---

## 6. Portability Assumptions

| Assumption | Description | Porting Impact |
|------------|-------------|----------------|
| P-01 | Target has at least 2 UARTs | Need 2 SERCOMs or software UART |
| P-02 | Target has SPI master capability | Required for ADC |
| P-03 | Target has at least 6 GPIO pins | LED + 3 range + CS + spare |
| P-04 | Target has hardware timer | For 1 ms tick |
| P-05 | Target supports 115200 baud | Standard rate |
| P-06 | Target supports 1 MHz SPI | MCP3204 requirement |

---

## 7. Mock Implementation Strategy

### 7.1 Mock File Structure

```
test/
  mocks/
    hal_gpio_mock.c     # GPIO mock with state tracking
    hal_spi_mock.c      # SPI mock with configurable responses
    hal_uart_mock.c     # UART mock with TX capture, RX injection
    hal_tick_mock.c     # Tick mock with time control
```

### 7.2 Mock Capabilities

| HAL Module | Mock Capability |
|------------|-----------------|
| hal_gpio   | Track pin states; verify writes; inject reads |
| hal_spi    | Configurable RX data; capture TX data |
| hal_uart   | Capture TX strings; inject RX sequences |
| hal_tick   | Control time advancement; trigger tick flags |

### 7.3 Example Mock Usage (hal_uart)

```c
// In test setup
hal_uart_mock_inject_rx(HAL_UART_BLE, "%STREAM_OPEN%\r\n");

// In test
ble_event_t event = ble_svc_poll();
assert(event == BLE_EVENT_STREAM_OPEN);

// Verify TX
const char* tx = hal_uart_mock_get_tx(HAL_UART_BLE);
assert(strstr(tx, "Meter") != NULL);
```

---

## 8. HAL Integration Checklist

Before declaring HAL integration complete:

- [ ] All four HAL modules have header files in `src/hal/`
- [ ] Target implementations compile with MCC drivers
- [ ] Mock implementations compile for host
- [ ] Each HAL function has unit test coverage via mocks
- [ ] No MCC types leak above HAL boundary
- [ ] Service layer compiles for both host and target
