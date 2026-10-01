# Architecture Overview

**Project:** Bluetooth Ohmmeter Firmware  
**Version:** 1.0  
**Date:** 2026-09-30  
**Status:** Draft

---

## 1. Executive Summary

This document describes the software architecture for a wireless resistance measurement device built on the PIC32CM3204GV00064 (ARM Cortex-M0+, 48 MHz, 32 KB Flash, 4 KB SRAM). The firmware acquires resistance measurements from an MCP3204 ADC via SPI, streams JSON-formatted data over Bluetooth Low Energy through an RNBD350 module, and provides visual LED feedback of connection state.

The architecture follows a **bare-metal cooperative super-loop** pattern with strict layering, information hiding, and downward-only dependencies. The design prioritizes testability via host-side unit testing with hardware abstraction, minimal memory footprint, and deterministic timing to meet the 100 ms sample interval requirement.

---

## 2. Architectural Style

### 2.1 Runtime Model

**Bare-metal cooperative super-loop** (no RTOS).

Rationale:
- Simplicity: single thread of execution eliminates concurrency bugs
- Determinism: no scheduler jitter; timing controlled by explicit polling
- Resource efficiency: no RTOS overhead (stack per task, TCBs)
- Project timeline: 2-day demo deadline favors minimal complexity

The main loop polls:
1. System tick (1 ms timer ISR sets flag; main loop acts on it)
2. UART RX for BLE status messages
3. State transitions for LED and data transmission

### 2.2 Layering Strategy

```
+--------------------------------------------------+
|  APPLICATION LAYER                               |
|  main.c, app_state.c                             |
|  - Super-loop orchestration                      |
|  - Application state machine                     |
+--------------------------------------------------+
        |
        v (calls)
+--------------------------------------------------+
|  SERVICE LAYER                                   |
|  measurement_svc.c, ble_svc.c, led_svc.c,        |
|  diag_svc.c                                      |
|  - Domain logic (ADC-to-ohms, JSON formatting)   |
|  - Protocol handling (RNBD350 parser)            |
|  - Timing policy (sample interval, LED rates)    |
+--------------------------------------------------+
        |
        v (calls)
+--------------------------------------------------+
|  HAL (Hardware Abstraction Layer)                |
|  hal_gpio.h, hal_spi.h, hal_uart.h, hal_tick.h   |
|  - Portable C function interfaces                |
|  - Synchronous, blocking where acceptable        |
|  - Mock-able for host-side unit tests            |
+--------------------------------------------------+
        |
        v (calls)
+--------------------------------------------------+
|  DRIVER LAYER (MCC-generated)                    |
|  sercom_spi.c, sercom_usart.c, gpio.c, tc.c      |
|  - Vendor-provided, unmodified                   |
|  - Target-specific register access               |
+--------------------------------------------------+
        |
        v (accesses)
+--------------------------------------------------+
|  PLATFORM / MCU HARDWARE                         |
|  PIC32CM3204GV00064 peripherals                  |
+--------------------------------------------------+
```

### 2.3 Concurrency Model

| Mechanism       | Usage                                           |
|-----------------|-------------------------------------------------|
| ISR (TC0)       | Increments 1 ms tick counter                    |
| ISR (SERCOM0 RX)| Buffers incoming bytes from RNBD350             |
| ISR (SERCOM2 TX)| Optional: background diagnostic TX (if used)    |
| Main loop       | All application logic; polls timestamps/buffers |

**ISR constraints:**
- ISRs set flags only; no business logic
- ISR-to-main communication via single-writer atomic flags or ring buffers
- No priority inversion concern (no blocking primitives)

---

## 3. Timing and Real-Time Constraints

| Constraint              | Value        | Source        |
|-------------------------|--------------|---------------|
| Sample interval         | 100 ms       | Req 3.1.3     |
| Sample jitter tolerance | +/-1 ms      | Req 5.3       |
| LED toggle (disconnected)| 1000 ms     | Req 3.1.6     |
| LED toggle (connected)  | 200 ms       | Req 3.1.6     |
| UART baud (BLE)         | 115200       | Req 2.3       |
| SPI clock (ADC)         | 1 MHz        | Req 2.3       |

**Timing budget per 100 ms tick:**

| Activity                    | Est. Time   |
|-----------------------------|-------------|
| SPI ADC read (24 bits @ 1 MHz) | ~30 us   |
| ADC-to-ohms conversion      | ~5 us       |
| JSON formatting (~32 bytes) | ~10 us      |
| UART TX (32 bytes @ 115200) | ~2.8 ms     |
| RNBD350 RX parsing          | ~50 us      |
| LED toggle                  | ~1 us       |
| **Total**                   | **< 5 ms**  |

Margin: ~95 ms available for future expansion or worst-case ISR latency.

---

## 4. Quality Attributes

### 4.1 Testability (Primary)

- **HAL abstraction**: All hardware access through `hal_*.h` interfaces
- **Host-side testing**: Services compile for host (x86) with mock HAL
- **Coverage target**: >= 95% line coverage on application code

### 4.2 Portability (Secondary)

- **HAL boundary**: Enables porting to alternative MCU (e.g., STM32, nRF52) by reimplementing HAL
- **MCC isolation**: Application code does not call MCC APIs directly

### 4.3 Determinism

- **Fixed timing**: No dynamic scheduling; all timing derived from 1 ms tick
- **No heap**: All memory statically allocated; no fragmentation or allocation latency
- **Bounded loops**: All loops have known upper bounds

### 4.4 Maintainability

- **Information hiding**: Each module encapsulates one design secret
- **Single responsibility**: Modules have focused, non-overlapping concerns
- **Compile-time configuration**: Diagnostic verbosity, sample interval

### 4.5 Resource Efficiency

| Resource | Budget  | Expected Usage |
|----------|---------|----------------|
| Flash    | 32 KB   | < 20 KB (including MCC) |
| RAM      | 4 KB    | < 2 KB (buffers + stack) |
| Stack    | ~1 KB   | Measured worst-case |

---

## 5. Key Architectural Decisions (Summary)

| ID       | Decision                          | Rationale                            |
|----------|-----------------------------------|--------------------------------------|
| ADR-0001 | Bare-metal super-loop             | Simplicity, determinism, timeline    |
| ADR-0002 | Thin synchronous HAL              | Testability, portability balance     |
| ADR-0003 | JSON streaming (no buffering)     | Memory efficiency, immediate TX      |
| ADR-0004 | RNBD350 transparent UART          | Simplicity over custom GATT          |
| ADR-0005 | Static allocation only            | Determinism, safety                  |

See `adr-specs.md` for full context and trade-offs.

---

## 6. Module Inventory (Summary)

| Module ID | Name              | Layer       | Purpose                              |
|-----------|-------------------|-------------|--------------------------------------|
| M-01      | main              | Application | Super-loop entry, initialization     |
| M-02      | app_state         | Application | Connection state machine             |
| M-03      | measurement_svc   | Service     | ADC read + conversion                |
| M-04      | ble_svc           | Service     | RNBD350 parser + TX                  |
| M-05      | led_svc           | Service     | LED timing logic                     |
| M-06      | diag_svc          | Service     | Diagnostic logging                   |
| M-07      | hal_gpio          | HAL         | GPIO abstraction                     |
| M-08      | hal_spi           | HAL         | SPI abstraction                      |
| M-09      | hal_uart          | HAL         | UART abstraction                     |
| M-10      | hal_tick          | HAL         | System tick abstraction              |

See `module-decomposition.md` for full details.

---

## 7. External Interfaces

| Interface        | Protocol       | Peer                  | Direction |
|------------------|----------------|-----------------------|-----------|
| SERCOM0 UART     | 115200 8N1     | RNBD350 BLE module    | Bidir     |
| SERCOM2 UART     | 115200 8N1     | Debug terminal        | TX only   |
| SERCOM5 SPI      | 1 MHz Mode 0   | MCP3204 ADC           | Bidir     |
| GPIO (3 pins)    | Digital out    | CD4028B range decoder | Out       |
| GPIO (1 pin)     | Digital out    | Status LED            | Out       |
| GPIO (1 pin)     | Digital out    | SPI chip select       | Out       |

---

## 8. Assumptions

| ID   | Assumption                                                                 | Impact if Wrong                     |
|------|----------------------------------------------------------------------------|-------------------------------------|
| A-01 | RNBD350 module is pre-configured (default baud, BLE settings)              | Need init sequence                  |
| A-02 | STREAM_OPEN event reliably indicates transparent UART mode                 | Need timeout/fallback               |
| A-03 | MCC drivers are stable and do not require modification                     | Need workaround shims               |
| A-04 | Conversion formula matches Multimeter Click documentation                  | Need calibration adjustment         |
| A-05 | Single measurement range (0-20k ohm) is sufficient                         | Need range-switching logic          |
| A-06 | No power management required (USB-powered demo)                            | Need sleep mode handling            |

---

## 9. Risks and Mitigations

| Risk                                      | Likelihood | Impact | Mitigation                          |
|-------------------------------------------|------------|--------|-------------------------------------|
| RNBD350 parser edge cases                 | Medium     | High   | Fuzz-test parser with captured logs |
| Timing overrun in BLE TX path             | Low        | Medium | Profile on target; add WCET checks  |
| MCC driver bugs                           | Low        | High   | Isolate via HAL; test on target early|
| Integration issues on 2026-10-02 demo     | Medium     | High   | Incremental integration; daily builds|

---

## 10. Document References

| Document                              | Location                                    |
|---------------------------------------|---------------------------------------------|
| Product Brief                         | `docs/requirements/product-brief.md`        |
| User Stories                          | `docs/requirements/user-stories.md`         |
| Module Decomposition                  | `docs/architecture/module-decomposition.md` |
| HAL Boundary                          | `docs/architecture/hal-boundary.md`         |
| Dependency Map                        | `docs/architecture/dependency-map.md`       |
| C4 Diagram Spec                       | `docs/architecture/c4-diagram-spec.md`      |
| ADR Specs                             | `docs/architecture/adr-specs.md`            |
