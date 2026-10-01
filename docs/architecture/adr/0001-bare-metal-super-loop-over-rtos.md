# ADR-0001: Bare-metal Super-loop Over RTOS

**Date:** 2026-09-30  
**Status:** Proposed  
**Authors:** Firmware Architecture Team  
**Priority:** High

---

## Context

The Bluetooth Ohmmeter firmware runs on a PIC32CM3204GV00064 microcontroller with the following specifications:

- **CPU:** ARM Cortex-M0+ at 48 MHz
- **Flash:** 32 KB
- **SRAM:** 4 KB

The firmware must perform the following concurrent operations:

1. Sample resistance at exactly 100 ms intervals (+/- 1 ms tolerance)
2. Parse incoming BLE status messages from the RNBD350 module
3. Transmit JSON-formatted measurement data when streaming is active
4. Toggle an LED at variable rates based on connection state
5. Output diagnostic messages without affecting measurement timing

The project operates under a 2-day implementation timeline with a demonstration scheduled for 2026-10-02. The development team has significant experience with bare-metal embedded systems.

### Forces and Constraints

| Force | Weight | Notes |
|-------|--------|-------|
| Timeline pressure | High | Must complete in 2 days |
| Determinism requirement | High | 100 ms sample interval +/- 1 ms |
| Memory constraints | High | 4 KB SRAM total |
| Team expertise | Medium | Developer familiar with bare-metal |
| Code complexity | Medium | No ISR-to-task synchronization needed |
| Future extensibility | Low | Single-purpose demo device |

---

## Decision

We will use a **bare-metal super-loop architecture** for the firmware execution model.

The architecture follows this pattern:

```
ISRs:        Set flags only (TC0 tick, UART RX)
Main Loop:   Poll flags and execute corresponding handlers
             - Check tick flag -> run measurement state machine
             - Check UART RX -> parse BLE status messages
             - Check TX ready -> transmit pending data
```

All significant processing occurs in the main loop context. Interrupt service routines are limited to setting flags and buffering received UART data.

---

## Alternatives Considered

### Option B: Lightweight RTOS (FreeRTOS)

Separate tasks would handle measurement, BLE communication, LED control, and diagnostics. The RTOS scheduler would manage task priorities and context switching.

**Advantages:**
- Clean separation of concerns through tasks
- Built-in priority-based scheduling
- Standard patterns for inter-task communication

**Disadvantages:**
- 2-4 KB RAM overhead for task stacks and kernel structures (50-100% of available SRAM)
- Integration and configuration time incompatible with 2-day timeline
- Learning curve for RTOS debugging
- Potential priority inversion issues require careful design

**Rejected because:** RAM overhead alone would consume half the available SRAM, and integration time would exceed the project timeline.

### Option C: Hybrid Timer-Driven State Machine

A TC0 ISR would run a minimal state machine for time-critical operations while the main loop handles slower operations.

**Advantages:**
- Guaranteed deterministic execution for critical path
- Lower overhead than full RTOS

**Disadvantages:**
- Complex ISR logic is difficult to debug
- State machine split across ISR and main contexts creates cognitive overhead
- Race conditions possible between ISR and main loop access to shared state

**Rejected because:** Complexity outweighs benefits given that all operations complete well within the 100 ms timing budget.

---

## Consequences

### Positive Consequences

1. **Simplicity:** Single execution context eliminates synchronization complexity
2. **Determinism:** No scheduler jitter or priority inversion possible
3. **Memory efficiency:** No RTOS overhead; all 4 KB SRAM available for application
4. **Implementation speed:** Developer can implement immediately without RTOS learning curve
5. **Debuggability:** Straightforward stack trace and execution flow
6. **Timing margin:** All operations complete in approximately 5 ms worst case, leaving 95 ms headroom per cycle

### Negative Consequences

1. **Blocking hazard:** Must carefully audit all code paths to ensure no blocking calls
2. **Priority management:** LED and diagnostic TX must be designed to not starve measurement loop
3. **Scalability limit:** Future features (OTA updates, multi-channel measurement) may require RTOS migration
4. **No preemption:** Long-running operations could delay time-sensitive tasks if not properly structured

### Risks and Mitigations

| Risk | Mitigation |
|------|------------|
| Blocking call in subsystem | Code review checklist; static analysis for blocking patterns |
| Measurement jitter | Measure actual timing during integration; TC0 interrupt has highest priority |
| Main loop starvation | Limit diagnostic TX to one message per loop iteration |

---

## Related Decisions

- **[ADR-0005: Static Allocation Policy](0005-static-allocation-policy.md)** - Static allocation complements the super-loop model by eliminating allocation-related latency and enabling deterministic memory usage

---

## Implementation Notes

1. **Timestamp-based Timing:** TC0 ISR increments a 32-bit millisecond counter. Main loop computes elapsed time via `now - last` (unsigned subtraction handles wraparound).

2. **UART RX Handling:** UART RX ISR writes to ring buffer. Main loop polls buffer and parses when data available.

3. **Non-blocking TX:** All UART TX operations must check TX ready status and return immediately if not ready.

4. **LED Timing:** LED toggle uses elapsed time accumulator; does not block.

---

## References

- Product Brief: docs/product-brief.md (Section 2: Technical Requirements)
- User Stories: docs/user-stories.md (US-002: Real-time Measurement Display)
- Architecture Overview: docs/architecture/architecture-overview.md (Section 4: Runtime Behavior)
