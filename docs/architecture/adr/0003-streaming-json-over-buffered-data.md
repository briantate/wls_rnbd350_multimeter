# ADR-0003: Streaming JSON Over Buffered Data

**Date:** 2026-09-30  
**Status:** Proposed  
**Authors:** Firmware Architecture Team  
**Priority:** Medium

---

## Context

The Bluetooth Ohmmeter must transmit resistance measurements to a connected BLE client at 100 ms intervals. The transmission format is:

```json
{"Meter":{"ohm":<value>}}
```

followed by `\r\n` (carriage return, line feed).

A critical design question is how to handle the TX path when the BLE UART link may not keep pace with measurement generation. This can occur during:

- BLE connection setup/teardown
- RF interference causing retransmissions
- Client-side processing delays

The design must balance several competing concerns:

| Concern | Requirement |
|---------|-------------|
| Data freshness | Users expect to see the current resistance, not stale queued values |
| Memory efficiency | 4 KB SRAM budget must accommodate all firmware needs |
| Reliability | Partial JSON messages must not corrupt the data stream |
| Simplicity | 2-day implementation timeline |

### Timing Analysis

At 115200 baud (8N1), one byte takes approximately 87 microseconds:

| Item | Size | TX Time |
|------|------|---------|
| Typical JSON message | 28 bytes | ~2.4 ms |
| Maximum JSON message | 32 bytes | ~2.8 ms |
| Available time per cycle | 100 ms | - |
| **Headroom** | - | **97+ ms** |

This analysis shows that under normal conditions, TX completes well before the next measurement is ready.

---

## Decision

We will use **immediate streaming without buffering**. Each measurement is formatted directly into a TX buffer and transmitted immediately. If the TX path is not ready, the measurement is dropped.

### Behavior Specification

```
On each 100ms tick:
  1. Read ADC and calculate resistance
  2. Check if UART TX is ready
  3. If ready: format JSON and transmit
  4. If not ready: drop measurement, optionally log diagnostic
  5. Continue to next tick
```

### Buffer Strategy

A single 48-byte static buffer is used for JSON formatting:

```c
static char tx_buffer[48];  // Sized for max message + margin
```

This buffer is overwritten on each measurement cycle. No queue, no double buffer.

---

## Alternatives Considered

### Option B: Ring Buffer Queue

Measurements would be queued in a ring buffer. A background TX process would drain the queue.

```
Measurement -> Ring Buffer -> TX Drain -> UART
```

**Advantages:**
- No dropped samples during brief congestion
- Complete data record for logging/analysis

**Disadvantages:**
- During sustained congestion, user sees stale data (potentially several seconds old)
- RAM overhead: 10-sample queue at 32 bytes = 320 bytes
- Added complexity for queue management
- Must handle queue overflow anyway

**Rejected because:** Users want current values, not historical queue. If BLE is congested for 1+ seconds, a 10-deep queue just delays showing current state.

### Option C: Double Buffer with Swap

Two measurement buffers; TX reads one while application writes the other.

```
Buffer A: [TX reading]  <-> Buffer B: [App writing]
          (swap on completion)
```

**Advantages:**
- No tearing (partial reads of in-progress writes)
- Single-sample latency

**Disadvantages:**
- Still only 1 sample deep; congestion still loses data
- Swap logic adds complexity
- Double the buffer RAM (96 bytes vs 48 bytes)
- Tearing is not a real risk with single-context execution

**Rejected because:** Complexity provides no benefit over Option A in a super-loop architecture where TX and measurement never execute concurrently.

---

## Consequences

### Positive Consequences

1. **Minimal RAM:** Single 48-byte buffer (vs 320+ bytes for queue)
2. **Always fresh:** User always sees the most recent measurement
3. **Simple implementation:** No queue management, no buffer swapping
4. **Predictable timing:** TX time is bounded and deterministic
5. **Clean failure mode:** Dropped samples create gaps, not stale data

### Negative Consequences

1. **Data loss during congestion:** Samples are dropped when TX not ready
2. **No recovery:** Dropped values cannot be reconstructed or replayed
3. **Detection burden:** Client must detect gaps (missing expected timestamps)
4. **Diagnostic overhead:** Logging dropped samples consumes some bandwidth

### Dropped Sample Handling

| Scenario | Expected Behavior |
|----------|-------------------|
| Normal operation | All samples transmitted |
| BLE connect/disconnect | 1-3 samples may be dropped |
| Sustained RF interference | Multiple samples dropped; client sees gaps |
| BLE link failure | All samples dropped until reconnect |

The diagnostic channel can output a message when samples are dropped:

```
[WARN] Measurement dropped: TX not ready
```

---

## Related Decisions

- **[ADR-0004: RNBD350 Transparent UART Over Custom GATT](0004-rnbd350-transparent-uart-over-custom-gatt.md)** - Transparent UART mode treats JSON as a simple serial stream, making streaming natural
- **[ADR-0005: Static Allocation Policy](0005-static-allocation-policy.md)** - The 48-byte TX buffer is statically allocated, contributing to known memory footprint

---

## Implementation Notes

1. **TX Ready Check:** Before formatting, check `hal_uart_tx_ready()`. If false, skip formatting entirely (saves CPU cycles).

2. **Atomic Message Guarantee:** Format the complete JSON message before starting TX. Never transmit partial messages.

3. **Diagnostic Rate Limiting:** If logging dropped samples, use a counter to avoid flooding diagnostics:
   ```c
   static uint16_t drop_count = 0;
   if (++drop_count >= 10) {
       diag_printf("[WARN] %u measurements dropped\n", drop_count);
       drop_count = 0;
   }
   ```

4. **JSON Format Validation:** The format string should be verified at compile time or during initialization:
   ```c
   #define JSON_FMT "{\"Meter\":{\"ohm\":%lu}}\r\n"
   ```

---

## References

- Product Brief: docs/product-brief.md (Section 2: Data Format Requirements)
- User Stories: docs/user-stories.md (US-002: Real-time Measurement Display)
- Architecture Overview: docs/architecture/architecture-overview.md (Section 3: Data Flow)
