---
name: pattern-thin-hal
description: Thin synchronous HAL with four surfaces (gpio/spi/uart/tick) for host-testable embedded firmware
metadata:
  type: reference
---

Thin synchronous HAL pattern for embedded projects needing host-side unit testing.

**Four standard surfaces:**
1. hal_gpio — pin read/write/toggle with logical pin IDs
2. hal_spi — synchronous transfer (single byte and block)
3. hal_uart — channel-based TX/RX with polling status
4. hal_tick — millisecond counter and tick flag

**Design rules:**
- All functions synchronous/blocking
- No MCC/vendor types in public headers (stdint.h only)
- No inter-HAL dependencies
- Mock implementations trivial (state tracking + injection)
- Target implementations call vendor drivers

**Interface pattern:**
```c
void hal_xxx_init(void);          // Initialize peripheral
// + operation-specific functions
```

**Portability boundary:**
- Above HAL: compiles for host AND target
- Below HAL: target-specific only

**When to use:**
- 95%+ unit test coverage target
- Potential future MCU porting
- Operations complete in microseconds (no need for async)

**When NOT to use:**
- DMA-heavy designs
- High-throughput streaming
- Complex interrupt-driven protocols

Related: [[pattern-bare-metal-superloop]], [[project-ble-ohmmeter]]
