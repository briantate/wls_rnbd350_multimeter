# ADR-0002: Thin Synchronous HAL Design

**Date:** 2026-09-30  
**Status:** Proposed  
**Authors:** Firmware Architecture Team  
**Priority:** High

---

## Context

The Bluetooth Ohmmeter firmware requires hardware abstraction to achieve three goals:

1. **Testability:** Enable host-side unit testing with mock implementations (95% coverage target)
2. **Portability:** Allow potential porting to alternative MCUs (e.g., STM32, nRF52)
3. **Isolation:** Protect application code from MCC (MPLAB Code Configurator) driver API changes

Four peripherals require abstraction:

| Peripheral | Primary Use | HAL Functions |
|------------|-------------|---------------|
| GPIO | LED control, status signals | `hal_gpio_write()`, `hal_gpio_read()` |
| SPI | MCP3204 ADC communication | `hal_spi_transfer()` |
| UART | BLE module and diagnostics | `hal_uart_tx()`, `hal_uart_rx_byte()`, `hal_uart_rx_available()` |
| Timer | 1 ms timebase | `hal_tick_init()`, `hal_tick_get_ms()` |

### Definition of "Synchronous"

The HAL API is **synchronous from the caller's perspective**: no callbacks, no async completion tokens, no futures. Functions return when the operation is complete or the status is known.

However, the **implementation** may use ISRs and internal buffers to prevent data loss. Specifically:

- UART RX uses interrupt-driven reception into ring buffers
- API remains poll-based: `hal_uart_rx_available()` and `hal_uart_rx_byte()`
- UART TX is polling-based (sufficient for throughput requirements)

This approach combines a simple API with reliable handling of asynchronous BLE events.

### Forces and Constraints

| Force | Weight | Notes |
|-------|--------|-------|
| Testability | High | 95% coverage target requires host testing |
| Implementation speed | High | 2-day timeline |
| Portability | Medium | Possible future MCU change |
| Code size | Medium | Flash budget 32 KB |
| Abstraction overhead | Low | Performance not critical at this scale |

---

## Decision

We will implement a **thin synchronous Hardware Abstraction Layer** with the following characteristics:

1. **Simple function-per-operation API** - Each hardware operation maps to a single function call
2. **Synchronous caller interface** - No callbacks; functions return results directly
3. **ISR-backed RX buffering** - UART RX uses interrupt-driven ring buffers internally
4. **Polling-based TX** - UART TX checks readiness and transmits directly
5. **Minimal state** - HAL maintains only essential state (ring buffer pointers, initialization flags)

### API Summary

```c
// GPIO
void hal_gpio_write(hal_pin_t pin, bool state);
bool hal_gpio_read(hal_pin_t pin);

// SPI
void hal_spi_transfer(const uint8_t* tx, uint8_t* rx, size_t len);

// UART
void hal_uart_tx_byte(hal_uart_t channel, uint8_t byte);
void hal_uart_tx(hal_uart_t channel, const uint8_t* data, size_t len);
bool hal_uart_tx_ready(hal_uart_t channel);
bool hal_uart_rx_available(hal_uart_t channel);
bool hal_uart_rx_byte(hal_uart_t channel, uint8_t* byte);

// Timer
void hal_tick_init(void);
uint32_t hal_tick_get_ms(void);
```

---

## Alternatives Considered

### Option B: Thick HAL with Callbacks

A callback-based asynchronous API where operations complete via registered callbacks.

```c
// Example callback-based API
void hal_uart_tx_async(hal_uart_t ch, const uint8_t* data, size_t len, 
                       void (*callback)(void* ctx), void* ctx);
```

**Advantages:**
- Scalable to high-throughput scenarios
- DMA-ready architecture
- Non-blocking by design

**Disadvantages:**
- Significantly more complex to implement and test
- Mock implementations must simulate callback timing
- Callback management adds code size and complexity
- 2-day timeline does not accommodate this complexity

**Rejected because:** Callback complexity is unjustified for the throughput requirements, and mock implementations become non-trivial.

### Option C: OS Abstraction Layer (OSAL)

A full operating system abstraction including threading primitives, event queues, and semaphores.

**Advantages:**
- Maximum portability across RTOS and bare-metal environments
- Standard patterns from embedded industry

**Disadvantages:**
- Massive overkill for single-threaded super-loop architecture
- Significant implementation and testing overhead
- Adds abstraction layers that provide no value for this project

**Rejected because:** The project uses bare-metal super-loop (ADR-0001), making OSAL entirely unnecessary.

---

## Consequences

### Positive Consequences

1. **Trivial mocking:** Mock implementations simply inject RX data and capture TX data
2. **Fast implementation:** Four small modules can be completed within timeline
3. **Clear contract:** Synchronous API is easy to understand and document
4. **Reliable RX:** ISR + ring buffer handles async BLE events without data loss
5. **Easy porting:** New MCU port requires only reimplementing 4 modules
6. **Testable in isolation:** Application logic can be tested without hardware

### Negative Consequences

1. **No DMA support:** Cannot leverage DMA without HAL redesign
2. **Throughput ceiling:** High-throughput serial scenarios not supported (acceptable for this use case)
3. **Implementation coupling:** ISR implementation details leak into HAL module (not API)
4. **Memory overhead:** UART RX ring buffers consume ~96 bytes RAM (see ADR-0005)

### Memory Impact

| Buffer | Size | Purpose |
|--------|------|---------|
| BLE UART RX ring | 64 bytes | Buffer incoming BLE status messages |
| DIAG UART RX ring | 32 bytes | Buffer diagnostic input (if any) |
| Ring buffer metadata | ~16 bytes | Head/tail pointers, flags |
| **Total** | **~112 bytes** | Static allocation per ADR-0005 |

---

## Related Decisions

- **[ADR-0001: Bare-metal Super-loop Over RTOS](0001-bare-metal-super-loop-over-rtos.md)** - The thin HAL with polling complements the super-loop execution model; no need for async primitives
- **[ADR-0005: Static Allocation Policy](0005-static-allocation-policy.md)** - HAL ring buffers use static allocation, contributing to the known memory footprint

---

## Implementation Notes

1. **Mock Structure:** Create `hal_mock.c` that implements all HAL functions using test-injectable data:
   ```c
   // In test code
   hal_mock_inject_rx(HAL_UART_BLE, "%CONNECT%\r\n", 11);
   // ... run code under test ...
   assert(hal_mock_get_tx_count(HAL_UART_BLE) == expected);
   ```

2. **Ring Buffer Implementation:** Use power-of-2 sizes for efficient modulo via bitmask:
   ```c
   #define BLE_RX_BUF_SIZE 64  // Must be power of 2
   #define BLE_RX_BUF_MASK (BLE_RX_BUF_SIZE - 1)
   ```

3. **ISR Safety:** Ring buffer head pointer written only by ISR; tail pointer written only by main loop. No locking required.

4. **Initialization Order:** `hal_timer_init()` must be called before main loop starts. UART and SPI can be initialized lazily on first use.

---

## References

- Product Brief: docs/product-brief.md (Section 3: Testing Requirements)
- Module Decomposition: docs/architecture/module-decomposition.md (HAL Module section)
- HAL Boundary: docs/architecture/hal-boundary.md
