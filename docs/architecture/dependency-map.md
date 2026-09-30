# Dependency Map

**Project:** Bluetooth Ohmmeter Firmware  
**Version:** 1.0  
**Date:** 2026-09-30  
**Status:** Draft

---

## 1. Overview

This document defines the allowed and forbidden dependencies between modules in the Bluetooth Ohmmeter firmware. Strict adherence to these rules ensures:

1. **Testability:** Services can be tested without hardware
2. **Maintainability:** Changes propagate predictably
3. **No cycles:** Dependency graph is a DAG (directed acyclic graph)

---

## 2. Layering Rules

### 2.1 Layer Hierarchy (Top to Bottom)

```
Layer 4: Application   (main, app_state)
             |
             v (may call)
Layer 3: Service       (measurement_svc, ble_svc, led_svc, diag_svc)
             |
             v (may call)
Layer 2: HAL           (hal_gpio, hal_spi, hal_uart, hal_tick)
             |
             v (may call)
Layer 1: Driver        (MCC-generated code)
             |
             v (accesses)
Layer 0: Hardware      (MCU peripherals)
```

### 2.2 Dependency Direction Rules

| Rule | Description |
|------|-------------|
| D-01 | Dependencies flow DOWNWARD only (higher layer calls lower layer) |
| D-02 | Same-layer dependencies allowed only within Application layer |
| D-03 | HAL modules are INDEPENDENT (no HAL-to-HAL dependencies) |
| D-04 | Services may NOT depend on other services |
| D-05 | Application layer orchestrates; services are leaf nodes |
| D-06 | No upward callbacks across layers (use polling or flags) |

---

## 3. Dependency Matrix

### 3.1 Module-to-Module Dependencies

Legend:
- **Y** = Allowed dependency (module in row calls module in column)
- **-** = No dependency allowed
- **X** = Forbidden (would violate layering)

| From \ To       | M-01 | M-02 | M-03 | M-04 | M-05 | M-06 | M-07 | M-08 | M-09 | M-10 |
|-----------------|------|------|------|------|------|------|------|------|------|------|
| **M-01 main**   | -    | Y    | Y    | Y    | Y    | Y    | -    | -    | -    | Y    |
| **M-02 app_state** | X | -    | -    | -    | -    | -    | -    | -    | -    | -    |
| **M-03 measurement_svc** | X | -  | -    | X    | X    | X    | Y    | Y    | -    | -    |
| **M-04 ble_svc** | X  | -    | X    | -    | X    | X    | -    | -    | Y    | -    |
| **M-05 led_svc** | X  | Y    | X    | X    | -    | X    | Y    | -    | -    | -    |
| **M-06 diag_svc** | X | -    | X    | X    | X    | -    | -    | -    | Y    | Y    |
| **M-07 hal_gpio** | X | X    | X    | X    | X    | X    | -    | -    | -    | -    |
| **M-08 hal_spi** | X  | X    | X    | X    | X    | X    | -    | -    | -    | -    |
| **M-09 hal_uart** | X | X    | X    | X    | X    | X    | -    | -    | -    | -    |
| **M-10 hal_tick** | X | X    | X    | X    | X    | X    | -    | -    | -    | -    |

### 3.2 Reading the Matrix

- **Row = "depends on"**: M-01 (main) depends on M-02, M-03, M-04, M-05, M-06, M-10
- **Column = "depended by"**: M-07 (hal_gpio) is depended on by M-03, M-05
- **X = Forbidden**: M-02 (app_state) must NOT depend on M-01 (main) - that would be upward
- **- = No relationship**: M-02 has no reason to call M-07 (app_state is pure logic)

---

## 4. Allowed Dependencies (Detailed)

### 4.1 Application Layer

#### M-01: main

| Depends On | Purpose |
|------------|---------|
| M-02 app_state | Query and update connection state |
| M-03 measurement_svc | Trigger samples, get measurements |
| M-04 ble_svc | Poll for BLE events, transmit data |
| M-05 led_svc | Update LED timing |
| M-06 diag_svc | Log diagnostic events |
| M-10 hal_tick | Check tick flag for timing |

#### M-02: app_state

| Depends On | Purpose |
|------------|---------|
| (none) | Pure state machine logic |

### 4.2 Service Layer

#### M-03: measurement_svc

| Depends On | Purpose |
|------------|---------|
| M-07 hal_gpio | Control RES_A/B/C for range, SPI_CS |
| M-08 hal_spi | Communicate with MCP3204 ADC |

#### M-04: ble_svc

| Depends On | Purpose |
|------------|---------|
| M-09 hal_uart | RX/TX with RNBD350 on HAL_UART_BLE |

#### M-05: led_svc

| Depends On | Purpose |
|------------|---------|
| M-02 app_state | Query streaming state for blink rate |
| M-07 hal_gpio | Toggle LED pin |

**Note:** M-05 depending on M-02 is an approved cross-layer exception. app_state provides read-only state; led_svc does not modify it.

#### M-06: diag_svc

| Depends On | Purpose |
|------------|---------|
| M-09 hal_uart | TX diagnostic messages on HAL_UART_DIAG |
| M-10 hal_tick | Get timestamp for log messages |

### 4.3 HAL Layer

All HAL modules are **independent** with no inter-HAL dependencies.

| Module | Depends On (below HAL) |
|--------|------------------------|
| M-07 hal_gpio | MCC GPIO driver |
| M-08 hal_spi | MCC SPI driver |
| M-09 hal_uart | MCC UART driver |
| M-10 hal_tick | MCC Timer driver |

---

## 5. Forbidden Dependencies

### 5.1 Upward Dependencies (Layer Violations)

| Forbidden | Reason |
|-----------|--------|
| Any module -> M-01 | main is the entry point; nothing calls it |
| HAL -> Service | HAL is below Service layer |
| HAL -> Application | HAL is below Application layer |
| Service -> Application | Service is below Application layer |
| Driver -> HAL | Driver is below HAL (but not modeled) |

### 5.2 Same-Layer Violations

| Forbidden | Reason |
|-----------|--------|
| Service -> Service | Creates coupling; orchestration belongs in main |
| HAL -> HAL | HAL modules must be independently testable |

### 5.3 Specific Forbidden Paths

| From | To | Why Forbidden |
|------|----|---------------|
| M-04 ble_svc | M-03 measurement_svc | Service-to-service; main passes measurements |
| M-03 measurement_svc | M-04 ble_svc | Service-to-service |
| M-05 led_svc | M-04 ble_svc | Service-to-service |
| M-06 diag_svc | M-04 ble_svc | Would mix diagnostic and BLE traffic |
| M-07 hal_gpio | M-08 hal_spi | HAL independence |

---

## 6. Approved Exceptions

| Exception | From | To | Justification |
|-----------|------|----|---------------|
| E-01 | M-05 led_svc | M-02 app_state | Read-only state query; simplifies main loop. app_state is essentially shared state. |

---

## 7. Dependency Graph (ASCII)

```
                           +--------+
                           |  main  |
                           | (M-01) |
                           +---+----+
                               |
       +-----------+-----------+-----------+-----------+
       |           |           |           |           |
       v           v           v           v           v
  +---------+ +---------+ +---------+ +---------+ +---------+
  |app_state| |meas_svc | |ble_svc  | |led_svc  | |diag_svc |
  | (M-02)  | | (M-03)  | | (M-04)  | | (M-05)  | | (M-06)  |
  +---------+ +----+----+ +----+----+ +----+----+ +----+----+
       ^           |           |           |           |
       |           |           |           |           |
       +-----------|-----------|-----------+           |
       (E-01)      |           |                       |
                   |           |                       |
              +----+----+ +----+----+              +---+---+
              |         | |         |              |       |
              v         v v         v              v       v
         +--------+ +--------+ +--------+    +--------+
         |hal_gpio| |hal_spi | |hal_uart|    |hal_tick|
         | (M-07) | | (M-08) | | (M-09) |    | (M-10) |
         +--------+ +--------+ +--------+    +--------+
              |         |           |             |
              v         v           v             v
         +--------------------------------------------+
         |            MCC Drivers (unmodified)        |
         +--------------------------------------------+
```

---

## 8. Cycle Prevention

### 8.1 Cycle Detection Method

Before adding any new dependency:
1. Trace the path from source to target
2. Trace all paths from target back toward source
3. If any path exists, the dependency creates a cycle

### 8.2 Current Cycle Analysis

**No cycles exist.** The dependency graph is verified acyclic.

Potential cycle risk: M-05 -> M-02 (approved exception E-01)
- M-02 has NO dependencies
- Therefore M-02 cannot reach M-05
- No cycle possible

---

## 9. Include Guard Verification

To mechanically verify dependencies, each module's `.c` file should include ONLY:

| Module | Allowed Includes |
|--------|------------------|
| main.c | app_state.h, measurement_svc.h, ble_svc.h, led_svc.h, diag_svc.h, hal_tick.h |
| app_state.c | app_state.h (own header only) |
| measurement_svc.c | measurement_svc.h, hal_gpio.h, hal_spi.h |
| ble_svc.c | ble_svc.h, hal_uart.h |
| led_svc.c | led_svc.h, app_state.h, hal_gpio.h |
| diag_svc.c | diag_svc.h, hal_uart.h, hal_tick.h |
| hal_*.c | Own header + MCC headers (target) or mock internals (host) |

---

## 10. Enforcement Checklist

- [ ] Static analysis tool configured to flag forbidden includes
- [ ] Code review checklist includes dependency verification
- [ ] Build system separates HAL implementations (target vs mock)
- [ ] Each service compiles independently (no implicit coupling)
- [ ] Module test files verify no hidden dependencies
