# ADR-0004: RNBD350 Transparent UART Over Custom GATT

**Date:** 2026-09-30  
**Status:** Proposed  
**Authors:** Firmware Architecture Team  
**Priority:** Medium

---

## Context

The RNBD350 BLE module supports two primary operating modes for data communication:

### Mode 1: Transparent UART

The module handles all BLE stack operations internally. The MCU communicates via a simple serial interface:

- MCU sends ASCII data to RNBD350 UART
- Module transmits data over BLE to connected client
- Client sees data as if reading from a serial port
- Status messages (`%CONNECT%`, `%DISCONNECT%`, `%STREAM_OPEN%`) indicate connection state

### Mode 2: Custom GATT Services

The MCU defines custom GATT characteristics. The module acts as a BLE radio:

- MCU issues AT-style commands to define services and characteristics
- MCU explicitly writes characteristic values
- Client reads/subscribes to specific characteristics
- Full control over BLE service architecture

### Project Requirements

The Bluetooth Ohmmeter must:

1. Transmit JSON-formatted measurement data: `{"Meter":{"ohm":<value>}}\r\n`
2. Work with off-the-shelf BLE terminal applications (no custom mobile app)
3. Be implemented within a 2-day timeline
4. Support real-time streaming at 100 ms intervals

### Forces and Constraints

| Force | Weight | Notes |
|-------|--------|-------|
| Implementation time | High | 2-day deadline |
| Interoperability | High | Must work with generic BLE terminals |
| Flexibility | Low | Fixed JSON format, no custom commands |
| Power efficiency | Low | USB-powered demo device |

---

## Decision

We will use **RNBD350 Transparent UART mode** for BLE communication.

The MCU treats the BLE link as a simple serial port:

```
MCU UART TX -> RNBD350 -> BLE -> Client App
MCU UART RX <- RNBD350 <- BLE <- Client App (status messages)
```

### Status Message Handling

The firmware must parse status messages to track connection state:

| Message | Meaning | Firmware Action |
|---------|---------|-----------------|
| `%CONNECT%` | BLE client connected | Update state; prepare for streaming |
| `%STREAM_OPEN%` | Transparent UART active | Begin transmitting measurements |
| `%DISCONNECT%` | BLE client disconnected | Stop streaming; return to advertising |

### Configuration

The RNBD350 is pre-configured (via MPLAB MCC or AT commands during setup) for:

- Transparent UART service enabled
- 115200 baud UART interface
- Device name: "BT-Ohmmeter" (or similar)
- Auto-advertise on power-up

---

## Alternatives Considered

### Option B: Custom GATT Service

Define a custom BLE service with a measurement characteristic:

```
Service: Ohmmeter Service (custom UUID)
  Characteristic: Resistance (custom UUID)
    Properties: Read, Notify
    Value: JSON string or binary value
```

**Advantages:**
- Clean BLE architecture following GATT patterns
- Could implement notifications for efficient updates
- More control over data format and timing
- Better integration with custom mobile applications

**Disadvantages:**
- Requires custom BLE client application or specific GATT explorer
- Significant MCU code for GATT command formatting
- More complex state machine for characteristic updates
- No standard BLE terminal app supports arbitrary custom characteristics well
- Implementation time exceeds project timeline

**Rejected because:** The requirement to work with off-the-shelf BLE terminal apps makes custom GATT impractical. Users should be able to connect with any serial-over-BLE terminal.

---

## Consequences

### Positive Consequences

1. **Minimal MCU code:** Just write JSON strings to UART; module handles BLE
2. **Universal compatibility:** Works with standard BLE terminal apps (Serial Bluetooth Terminal, nRF Toolbox, etc.)
3. **Fast implementation:** No GATT service design or characteristic management
4. **Natural fit for JSON:** Text-based protocol over text-based BLE serial
5. **Well-documented:** RNBD350 User Guide provides complete status message specification

### Negative Consequences

1. **No custom characteristics:** Cannot expose structured data as discrete GATT attributes
2. **Opaque segmentation:** BLE MTU and packet segmentation handled by module; MCU has no visibility
3. **Status parsing required:** Firmware must parse `%CONNECT%`, `%DISCONNECT%`, `%STREAM_OPEN%` messages
4. **Client-side parsing:** Client application must parse JSON from serial stream
5. **No bidirectional commands:** Complex command/response protocols would need custom framing

### Client Application Requirements

| Requirement | Solution |
|-------------|----------|
| BLE terminal app | Use any serial-over-BLE terminal (free apps available) |
| JSON parsing | Client displays raw text; user reads JSON directly for demo |
| Connection management | Standard BLE pairing and connection via terminal app |

---

## Related Decisions

- **[ADR-0003: Streaming JSON Over Buffered Data](0003-streaming-json-over-buffered-data.md)** - JSON streaming design is implemented directly over transparent UART; the serial abstraction makes this straightforward

---

## Implementation Notes

1. **Status Message Parser:** Implement a simple state machine to detect status message boundaries:
   ```c
   // States: IDLE, PERCENT_SEEN, PARSING_STATUS
   // On '%': transition to PERCENT_SEEN
   // On letters: accumulate status string
   // On '%' again: complete status message
   ```

2. **Initialization Sequence:** On power-up, wait for RNBD350 to complete initialization before sending data. The module outputs `%REBOOT%` on startup.

3. **Flow Control:** Transparent UART does not provide hardware flow control. Rely on BLE module's internal buffering and the timing analysis from ADR-0003.

4. **Error Handling:** If unexpected status messages arrive, log to diagnostic channel but do not halt operation.

5. **Configuration Persistence:** RNBD350 stores configuration in flash. Initial setup (device name, UART baud rate, etc.) only needed once per module.

### Typical Operation Sequence

```
1. Power on
2. RNBD350 outputs: %REBOOT%
3. RNBD350 begins advertising
4. Client connects
5. RNBD350 outputs: %CONNECT,<address>%
6. Client opens transparent UART service
7. RNBD350 outputs: %STREAM_OPEN%
8. MCU begins transmitting: {"Meter":{"ohm":1234}}\r\n
9. Client receives JSON stream
10. Client disconnects
11. RNBD350 outputs: %DISCONNECT%
12. Return to step 3
```

---

## References

- RNBD350 User Guide: Transparent UART Service section
- Product Brief: docs/product-brief.md (Section 2: Connectivity Requirements)
- User Stories: docs/user-stories.md (US-001: BLE Connection, US-002: Real-time Display)
- Module Decomposition: docs/architecture/module-decomposition.md (BLE Module section)
