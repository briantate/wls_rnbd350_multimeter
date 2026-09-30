---
name: project-ble-ohmmeter
description: Bluetooth Ohmmeter project on PIC32CM with RNBD350 BLE module, bare-metal super-loop, 100ms sampling
metadata:
  type: project
---

Bluetooth Ohmmeter firmware project: PIC32CM3204GV00064 (ARM Cortex-M0+, 48 MHz, 32 KB Flash, 4 KB SRAM) with RNBD350 BLE module and MCP3204 external ADC.

**Why:** Demo deadline 2026-10-02 (2-day implementation window), wireless resistance measurement for test engineers.

**How to apply:** Architecture favors simplicity over extensibility. Bare-metal super-loop chosen over RTOS. Static allocation only. Thin synchronous HAL for testability. JSON streaming without buffering.

Key ADRs:
- ADR-0001: Bare-metal super-loop over RTOS
- ADR-0002: Thin synchronous HAL
- ADR-0003: Streaming JSON (no buffer)
- ADR-0004: RNBD350 transparent UART mode
- ADR-0005: Static allocation only

Related: [[pattern-bare-metal-superloop]], [[pattern-thin-hal]]
