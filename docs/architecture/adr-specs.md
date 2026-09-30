# ADR Specifications

**Project:** Bluetooth Ohmmeter Firmware  
**Version:** 1.0  
**Date:** 2026-09-30  
**Status:** Draft  
**Consumer:** adr-expert agent

---

## 1. Purpose

This document specifies 5 architectural decisions for the Bluetooth Ohmmeter firmware. Each entry provides sufficient context, forces, and direction for adr-expert to expand into a full Architecture Decision Record.

---

## 2. ADR Summary Table

| ADR ID | Title | Status | Priority |
|--------|-------|--------|----------|
| ADR-0001 | Bare-metal super-loop over RTOS | Proposed | High |
| ADR-0002 | Thin synchronous HAL design | Proposed | High |
| ADR-0003 | Streaming JSON over buffered data | Proposed | Medium |
| ADR-0004 | RNBD350 transparent UART over custom GATT | Proposed | Medium |
| ADR-0005 | Static allocation policy | Proposed | High |

---

## 3. ADR-0001: Bare-metal super-loop over RTOS

### Status
Proposed

### Context

The Bluetooth Ohmmeter firmware runs on a PIC32CM3204GV00064 (ARM Cortex-M0+, 48 MHz, 32 KB Flash, 4 KB SRAM). The firmware must:
- Sample resistance at exactly 100 ms intervals
- Parse incoming BLE status messages
- Transmit JSON-formatted data when streaming
- Toggle an LED at variable rates based on connection state
- Output diagnostic messages without affecting measurement timing

The project has a 2-day implementation timeline (demo 2026-10-02).

### Forces and Constraints

| Force | Weight | Notes |
|-------|--------|-------|
| Timeline pressure | High | Must complete in 2 days |
| Determinism requirement | High | 100 ms sample interval +/- 1 ms |
| Memory constraints | High | 4 KB SRAM total |
| Team expertise | Medium | Developer familiar with bare-metal |
| Code complexity | Medium | No ISR-to-task synchronization needed |
| Future extensibility | Low | Single-purpose demo device |

### Options Considered

**Option A: Bare-metal super-loop**
- Infinite loop polls tick flag, UART RX, and state
- ISRs set flags only; main loop does work
- Pros: Simple, deterministic, no RTOS footprint, fast to implement
- Cons: Must ensure no blocking calls; manual priority management

**Option B: Lightweight RTOS (FreeRTOS)**
- Separate tasks for measurement, BLE, LED, diagnostics
- RTOS handles scheduling
- Pros: Clean separation, easier multi-priority scheduling
- Cons: 2-4 KB RAM overhead, learning curve, integration time

**Option C: Hybrid (timer-driven state machine)**
- TC0 ISR runs minimal state machine
- Main loop handles slow operations
- Pros: Deterministic critical path
- Cons: Complex ISR; difficult to debug

### Recommended Direction

**Option A: Bare-metal super-loop**

Rationale:
1. 2-day timeline does not allow RTOS integration and debugging
2. All operations complete well under 100 ms budget (~5 ms worst case)
3. Single-priority execution simplifies reasoning
4. Entire firmware fits comfortably in 4 KB SRAM without RTOS overhead
5. Developer can implement immediately without RTOS learning curve

### Consequences

- Must carefully avoid blocking calls in any subsystem
- LED timing and diagnostic TX must not starve measurement loop
- No priority inversion possible (single execution context)
- Future features (OTA, multi-channel) may require RTOS migration

---

## 4. ADR-0002: Thin synchronous HAL design

### Status
Proposed

### Context

The firmware requires hardware abstraction to enable:
1. Host-side unit testing with mock implementations
2. Potential porting to alternative MCU (e.g., STM32, nRF52)
3. Isolation from MCC driver API changes

Four peripherals need abstraction: GPIO, SPI, UART, Timer.

### Forces and Constraints

| Force | Weight | Notes |
|-------|--------|-------|
| Testability | High | 95% coverage target requires host testing |
| Implementation speed | High | 2-day timeline |
| Portability | Medium | Possible future MCU change |
| Code size | Medium | Flash budget 32 KB |
| Abstraction overhead | Low | Performance is not critical |

### Options Considered

**Option A: Thin synchronous HAL**
- Simple function-per-operation API (e.g., `hal_gpio_write()`)
- All functions blocking/synchronous
- No internal buffering or state
- Mock implementation trivial
- Pros: Simple, fast to implement, easy to test
- Cons: No async/DMA support; limited scalability

**Option B: Thick HAL with callbacks**
- Callback-based async API
- HAL manages buffers and interrupts
- Pros: Scalable, DMA-ready
- Cons: Complex, harder to mock, more code

**Option C: OS abstraction layer (OSAL)**
- Full OSAL with threading, events, queues
- Pros: Maximum portability
- Cons: Massive overkill for this project

### Recommended Direction

**Option A: Thin synchronous HAL**

Rationale:
1. All operations complete in microseconds; async not needed
2. Mock implementations become trivial stubs
3. Fastest to implement given timeline
4. MCP3204 SPI transfer is inherently blocking (24-bit exchange)
5. UART TX can be polling without impacting 100 ms budget

### Consequences

- Cannot use DMA without HAL redesign
- High-throughput serial not possible (acceptable for this use case)
- Porting requires only reimplementing 4 small modules
- Testing isolated from hardware completely

---

## 5. ADR-0003: Streaming JSON over buffered data

### Status
Proposed

### Context

Resistance measurements must be transmitted as `{"Meter":{"ohm":<value>}}\r\n` at 100 ms intervals. Options exist for how to handle the TX path when BLE UART may not keep up.

### Forces and Constraints

| Force | Weight | Notes |
|-------|--------|-------|
| Memory efficiency | High | 4 KB SRAM budget |
| Data freshness | High | Users want latest value |
| Simplicity | High | Timeline pressure |
| Reliability | Medium | Don't want partial JSON |

### Options Considered

**Option A: Immediate streaming (no buffer)**
- Format JSON directly into TX
- If TX not ready, drop measurement
- Pros: Minimal RAM, always latest value, simple
- Cons: May lose samples during BLE congestion

**Option B: Ring buffer queue**
- Queue measurements in ring buffer
- TX drains queue in background
- Pros: No dropped samples
- Cons: Stale data during congestion, RAM usage, complexity

**Option C: Double buffer with swap**
- Two measurement buffers; TX reads one while app writes other
- Pros: No tearing
- Cons: Still only 1 sample deep; adds complexity

### Recommended Direction

**Option A: Immediate streaming (no buffer)**

Rationale:
1. UART TX at 115200 baud takes ~2.8 ms for 32 bytes; plenty of headroom
2. BLE transparent UART can absorb bursts
3. If BLE is congested, users want latest value, not stale queue
4. Dropped samples are detectable (missing timestamp or gap in data)
5. Minimal RAM: single 48-byte format buffer

### Consequences

- If BLE link is saturated, samples are dropped (acceptable per requirements)
- No backpressure signaling needed
- Diagnostic log can note dropped samples
- Cannot reconstruct dropped values

---

## 6. ADR-0004: RNBD350 transparent UART over custom GATT

### Status
Proposed

### Context

The RNBD350 module supports two modes:
1. **Transparent UART**: Module handles BLE; MCU sees simple serial stream
2. **Custom GATT**: MCU defines GATT services; module is BLE radio only

The firmware must transmit JSON measurement data to a BLE client.

### Forces and Constraints

| Force | Weight | Notes |
|-------|--------|-------|
| Implementation time | High | 2-day deadline |
| Interoperability | High | Must work with generic BLE terminals |
| Flexibility | Low | Fixed JSON format, no custom commands |
| Power efficiency | Low | USB-powered demo |

### Options Considered

**Option A: Transparent UART mode**
- RNBD350 presents as serial port to BLE client
- MCU sends JSON over UART; module handles BLE framing
- Status messages (%CONNECT%, %STREAM_OPEN%) indicate state
- Pros: Trivial MCU-side code, works with standard BLE terminal apps
- Cons: No custom GATT characteristics, limited control

**Option B: Custom GATT service**
- Define custom characteristic for measurement data
- MCU issues GATT update commands
- Pros: Clean BLE architecture, could add notifications
- Cons: Requires custom mobile app or specific GATT client, more MCU code

### Recommended Direction

**Option A: Transparent UART mode**

Rationale:
1. Demo must work with off-the-shelf BLE terminal apps
2. JSON is inherently text-based; transparent UART is natural fit
3. MCU code is minimal: just write to serial port
4. Status message parsing is well-documented in RNBD350 user guide
5. No mobile app development required

### Consequences

- Cannot implement custom GATT characteristics
- BLE MTU/segmentation handled by module (opaque to MCU)
- Status message parser must handle %CONNECT%, %DISCONNECT%, %STREAM_OPEN%
- Client app must parse JSON from serial stream

---

## 7. ADR-0005: Static allocation policy

### Status
Proposed

### Context

The firmware runs on a constrained MCU with 4 KB SRAM. Memory allocation strategy affects determinism, safety, and debugging.

### Forces and Constraints

| Force | Weight | Notes |
|-------|--------|-------|
| Determinism | High | No allocation latency in timing path |
| Safety | High | No heap fragmentation |
| Memory efficiency | Medium | Static may waste unused space |
| Flexibility | Low | Fixed-size requirements known |

### Options Considered

**Option A: Static allocation only**
- All buffers declared as static arrays
- Sizes fixed at compile time
- Pros: Deterministic, no fragmentation, link-time size verification
- Cons: Must know max sizes, may waste unused space

**Option B: Heap allocation (malloc)**
- Dynamic allocation as needed
- Pros: Flexible, memory efficient for varying needs
- Cons: Fragmentation risk, non-deterministic timing, debug difficulty

**Option C: Fixed-block allocator**
- Pool of fixed-size blocks
- Pros: Deterministic, no fragmentation
- Cons: Overhead, complexity, still need to size pool

### Recommended Direction

**Option A: Static allocation only**

Rationale:
1. All buffer sizes are known at design time:
   - BLE RX parse buffer: 32 bytes
   - JSON TX buffer: 48 bytes
   - Diagnostic TX buffer: 128 bytes
   - UART RX ring buffers: 2 x 32 bytes
2. Total static allocation ~300 bytes; well under 4 KB
3. Linker map provides exact memory usage at build time
4. No heap means no fragmentation or allocation failures
5. Easier to certify for reliability

### Consequences

- Cannot add variable-size features without recompilation
- Memory "wasted" if buffers not fully used (acceptable)
- Linker will fail if total exceeds SRAM (early detection)
- No malloc/free in codebase; static analysis can verify

---

## 8. Expansion Notes for adr-expert

When expanding these specs into full ADRs:

1. **Add date and authors** to each ADR
2. **Expand consequences** with both positive and negative impacts
3. **Add "Related Decisions"** section linking to other ADRs where relevant
4. **Include "Notes"** section for implementation guidance
5. **Add "References"** to relevant sections of product-brief.md and user-stories.md
6. **Use standard ADR format** (Title, Status, Context, Decision, Consequences)

### Cross-References

| ADR | Related To |
|-----|------------|
| ADR-0001 | ADR-0005 (static allocation enables simple super-loop) |
| ADR-0002 | ADR-0001 (thin HAL suits super-loop model) |
| ADR-0003 | ADR-0005 (streaming avoids buffer queues) |
| ADR-0004 | ADR-0003 (transparent UART is a JSON stream) |
| ADR-0005 | ADR-0001, ADR-0002 (static allocation simplifies both) |
