---
name: pattern-bare-metal-superloop
description: Recurring module decomposition for single-actuator embedded systems with cooperative polling loop
metadata:
  type: reference
---

Bare-metal super-loop pattern for small embedded projects with slow sample periods (tens to hundreds of ms).

**Structure:**
- Application layer: main (orchestrator), app_state (state machine)
- Service layer: domain-specific services (measurement, comms, LED, diagnostics)
- HAL layer: hal_gpio, hal_spi, hal_uart, hal_tick
- Driver layer: vendor-provided (MCC, STM32 HAL, etc.)

**Layering rules:**
- Dependencies flow downward only
- Services independent (no service-to-service calls)
- HAL modules independent (no HAL-to-HAL calls)
- State machine can be queried cross-layer (read-only exception)

**When to use:**
- Timeline pressure (RTOS integration adds days)
- Memory-constrained (< 8 KB SRAM)
- Simple timing requirements (single dominant sample rate)
- All operations complete well under timing budget

**When NOT to use:**
- Multiple independent timing domains with different priorities
- Need for blocking operations in concurrent paths
- Complex inter-task synchronization

Related: [[pattern-thin-hal]], [[project-ble-ohmmeter]]
