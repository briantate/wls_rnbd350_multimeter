# ADR-0005: Static Allocation Policy

**Date:** 2026-09-30  
**Status:** Proposed  
**Authors:** Firmware Architecture Team  
**Priority:** High

---

## Context

The Bluetooth Ohmmeter firmware runs on a PIC32CM3204GV00064 with only 4 KB (4096 bytes) of SRAM. Memory allocation strategy has significant implications for:

| Concern | Impact |
|---------|--------|
| Determinism | Allocation timing affects measurement loop |
| Safety | Fragmentation can cause unpredictable failures |
| Debugging | Memory-related bugs are difficult to diagnose |
| Certification | Static memory is easier to analyze and verify |

The firmware requires buffers for:

- BLE status message parsing
- JSON measurement formatting
- Diagnostic message formatting  
- UART receive buffering (two channels)

### Memory Budget Analysis

| Component | Estimated Size |
|-----------|---------------|
| Stack | ~512 bytes |
| Global variables | ~64 bytes |
| Application buffers | ~320 bytes |
| HAL ring buffers | ~112 bytes |
| **Subtotal (application)** | **~1008 bytes** |
| MCC driver state | ~256 bytes (estimated) |
| **Total estimated** | **~1264 bytes** |
| **Available** | **4096 bytes** |
| **Margin** | **~2832 bytes (69%)** |

This analysis shows comfortable margin, but the margin could be consumed by heap fragmentation or unexpected stack growth.

### Forces and Constraints

| Force | Weight | Notes |
|-------|--------|-------|
| Determinism | High | No allocation latency in timing path |
| Safety | High | No heap fragmentation |
| Memory efficiency | Medium | Static may waste unused space |
| Flexibility | Low | Fixed-size requirements known at design time |

---

## Decision

We will use **static allocation exclusively**. No dynamic memory allocation (malloc, calloc, realloc, free) will be used anywhere in the firmware.

### Allocation Strategy

All buffers are declared as static arrays with sizes fixed at compile time:

```c
// Application buffers
static char json_tx_buffer[48];      // JSON formatting
static char diag_tx_buffer[128];     // Diagnostic messages
static char ble_parse_buffer[32];    // BLE status parsing

// HAL buffers (in hal_uart.c)
static uint8_t ble_rx_ring[64];      // BLE UART RX ring buffer
static uint8_t diag_rx_ring[32];     // Diagnostic UART RX ring buffer
static volatile uint8_t ble_rx_head, ble_rx_tail;
static volatile uint8_t diag_rx_head, diag_rx_tail;
```

### Verification Mechanism

Memory usage is verified at build time via linker map analysis:

```
Build output must include:
  .data section size
  .bss section size
  Stack allocation
  Total SRAM usage vs. available
```

---

## Alternatives Considered

### Option B: Heap Allocation (malloc/free)

Dynamic allocation as needed, with buffers allocated and freed during operation.

**Advantages:**
- Memory efficient when different features have non-overlapping lifetimes
- Flexible for varying-size data structures
- Familiar programming model

**Disadvantages:**
- **Fragmentation:** In long-running embedded systems, heap can become fragmented
- **Non-deterministic timing:** malloc may take variable time depending on heap state
- **Failure modes:** Allocation failure during operation is difficult to handle gracefully
- **Debug difficulty:** Memory leaks and corruption are notoriously hard to diagnose
- **No benefit:** All buffer sizes are known; dynamic allocation provides no advantage

**Rejected because:** Fragmentation and non-determinism are unacceptable in a measurement system, and the flexibility of dynamic allocation provides no benefit when requirements are fixed.

### Option C: Fixed-Block Allocator (Pool Allocator)

A pool of fixed-size blocks allocated at startup, dispensed as needed.

```c
// Example: pool of 32-byte blocks
static uint8_t block_pool[8][32];
static uint8_t block_used[8];

void* pool_alloc(void);
void pool_free(void* ptr);
```

**Advantages:**
- Deterministic allocation time (O(1))
- No fragmentation within pool
- Can track allocation patterns for debugging

**Disadvantages:**
- Still requires pool sizing (same analysis as static allocation)
- Overhead for block management (metadata, allocation functions)
- Added complexity without added capability
- Can still exhaust pool if misused

**Rejected because:** Provides no advantage over direct static allocation while adding management overhead.

---

## Consequences

### Positive Consequences

1. **Deterministic execution:** No allocation latency in any code path
2. **No fragmentation:** Impossible by design
3. **Link-time verification:** Linker fails if total exceeds SRAM capacity
4. **Analyzable memory map:** Exact usage known from linker output
5. **No runtime failures:** Cannot have allocation failure during operation
6. **Easier certification:** Static analysis tools can verify memory bounds
7. **Simplified debugging:** No memory allocator state to inspect

### Negative Consequences

1. **Inflexibility:** Cannot add variable-size features without recompilation
2. **Potential waste:** Unused buffer space cannot be reclaimed
3. **Design constraint:** Maximum sizes must be determined during design phase
4. **Compile-time coupling:** Buffer size changes require rebuild

### Static Allocation Summary

| Buffer | Size (bytes) | Location | Purpose |
|--------|-------------|----------|---------|
| `json_tx_buffer` | 48 | app_measure.c | JSON measurement formatting |
| `diag_tx_buffer` | 128 | diag.c | Diagnostic message formatting |
| `ble_parse_buffer` | 32 | ble_status.c | BLE status message parsing |
| `ble_rx_ring` | 64 | hal_uart.c | BLE UART receive ring buffer |
| `diag_rx_ring` | 32 | hal_uart.c | Diagnostic UART receive ring |
| Ring buffer metadata | 16 | hal_uart.c | Head/tail pointers, flags |
| **Total application buffers** | **320** | | |

---

## Related Decisions

- **[ADR-0001: Bare-metal Super-loop Over RTOS](0001-bare-metal-super-loop-over-rtos.md)** - Static allocation eliminates one source of non-determinism, complementing the deterministic super-loop model
- **[ADR-0002: Thin Synchronous HAL Design](0002-thin-synchronous-hal-design.md)** - HAL ring buffers are statically allocated per this policy
- **[ADR-0003: Streaming JSON Over Buffered Data](0003-streaming-json-over-buffered-data.md)** - Streaming approach avoids need for large message queues, fitting within static allocation budget

---

## Implementation Notes

1. **Linker Script Verification:** Configure linker to report error if .data + .bss exceeds SRAM:
   ```
   # In linker script or build settings
   ASSERT((_end_of_ram - _start_of_ram) <= 4096, "SRAM overflow")
   ```

2. **Static Analysis:** Use compiler warnings to catch dynamic allocation:
   ```c
   // In project headers
   #define malloc(x)  STATIC_ASSERT(0, "malloc prohibited")
   #define free(x)    STATIC_ASSERT(0, "free prohibited")
   #define calloc(x,y) STATIC_ASSERT(0, "calloc prohibited")
   ```

3. **Buffer Sizing Rationale:**
   - JSON TX (48 bytes): Maximum message `{"Meter":{"ohm":4294967295}}\r\n` is 35 bytes; 48 provides margin
   - Diagnostic TX (128 bytes): Longest diagnostic message with parameters
   - BLE parse (32 bytes): Longest status `%STREAM_OPEN%` is 13 bytes; 32 handles future expansion
   - BLE RX ring (64 bytes): Two full status messages can queue
   - Diag RX ring (32 bytes): Minimal input expected on diagnostic channel

4. **Memory Map Review:** Include memory map review in code review checklist. Any change affecting buffer sizes must update this ADR.

---

## Compliance Verification

| Method | Frequency | Responsibility |
|--------|-----------|----------------|
| Linker map review | Each build | Automated CI |
| Static analysis | Each commit | Automated CI |
| Code review | Each PR | Reviewer checklist item |
| ADR review | Design phase | Architect |

### Automated Check Example

```bash
# CI script to verify no malloc usage
grep -r "malloc\|calloc\|realloc\|free" src/ && exit 1 || exit 0
```

---

## References

- Product Brief: docs/product-brief.md (Section 2: Hardware Constraints)
- Architecture Overview: docs/architecture/architecture-overview.md (Section 5: Memory Model)
- Module Decomposition: docs/architecture/module-decomposition.md (Buffer allocation table)
