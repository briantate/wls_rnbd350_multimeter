# User Stories — Bluetooth Ohmmeter Firmware

**Source:** `docs/requirements/product-brief.md`
**Generated:** 2026-09-29
**Input layer:** Component / module spec

## Summary

Firmware for a wireless resistance measurement device that transmits real-time ohmmeter readings over Bluetooth Low Energy. The system acquires resistance measurements from an MCP3204 ADC at 100 ms intervals and streams JSON-formatted data to a connected BLE client via the RNBD350 module. Visual LED feedback indicates connection state, and diagnostic messages are output on a dedicated UART.

## Personas

- **Test Engineer** — uses the device to capture resistance measurements for automated test systems; needs reliable data streaming and consistent JSON format for parsing.
- **Field Technician** — requires portable resistance measurement with wireless data logging; needs clear visual feedback of device state and reliable operation without cable management.
- **Development Engineer** — monitors real-time resistance values during prototype testing; needs accurate readings and diagnostic output for troubleshooting.
- **Firmware Developer** — integrates and tests the firmware modules; needs clean interfaces, testable components, and documented APIs.

## Story map

- **E1-001 — Operator views real-time resistance measurements on connected device** _[User-facing, Product]_
  - E1-002 — Read resistance value from MCP3204 ADC _[Enabler, Module]_
  - E1-003 — Convert raw ADC counts to ohms _[Enabler, Module]_
  - E1-004 — Configure 0–20 kΩ measurement range _[Enabler, Module]_
- **E2-001 — Operator receives resistance data wirelessly via BLE** _[User-facing, Product]_
  - E2-002 — Initialize RNBD350 BLE module communication _[Enabler, Module]_
  - E2-003 — Parse RNBD350 status messages _[Enabler, Module]_
  - E2-004 — Detect STREAM_OPEN event to enable data transmission _[Enabler, Module]_
  - E2-005 — Format measurement as JSON and transmit _[Enabler, Module]_
  - E2-006 — Handle BLE disconnect and stop transmission _[Enabler, Module]_
- **E3-001 — Operator sees LED feedback indicating connection state** _[User-facing, Product]_
  - E3-002 — Toggle LED at 1 Hz when disconnected _[Enabler, Module]_
  - E3-003 — Toggle LED at 5 Hz when connected _[Enabler, Module]_
- **E4-001 — Developer diagnoses firmware behavior via diagnostic output** _[User-facing, Product]_
  - E4-002 — Output timestamped diagnostic messages on SERCOM2 _[Enabler, Module]_
  - E4-003 — Implement compile-time verbosity control _[Enabler, Module]_
- **E5-001 — System operates reliably under timing constraints** _[User-facing, System]_
  - E5-002 — Sample resistance at fixed 100 ms interval _[Enabler, Module]_
  - E5-003 — Handle missed samples without data accumulation _[Enabler, Module]_
- _Top-level technical/NFR stories (Epic E6):_
  - E6-001 — Bare-metal cooperative main loop architecture _[Technical, Component]_
  - E6-002 — Static memory allocation only _[Technical, Component]_
  - E6-003 — MCC driver integration without modification _[Technical, Component]_

## Definition of Ready

A story is Ready to be pulled into a sprint when:

- [ ] Acceptance criteria are reviewed and unambiguous.
- [ ] Persona and benefit are confirmed (not inferred placeholders).
- [ ] Open questions affecting this story are resolved or labeled as low-risk assumptions.
- [ ] Story is small enough to fit in a sprint.
- [ ] Dependencies are identified and the dependency story is itself Ready or Done.
- [ ] Estimate is agreed by the team.
- [ ] Required hardware documentation (datasheets, schematics) is available.
- [ ] HAL interface for the story's module is defined.

## Definition of Done

A story is Done when **all** of the following are true:

- [ ] Acceptance criteria pass on host with a test-double HAL/dependency.
- [ ] Coverage target met on the changed code (≥95% line coverage per success criteria).
- [ ] On-target smoke test passes on the Curiosity Nano hardware.
- [ ] Static analysis is clean (or new findings are recorded as deviations).
- [ ] Worst-case execution time measured and within 100 ms sample budget.
- [ ] RAM/Flash usage measured and within MCU limits (4 KB SRAM, 32 KB Flash).
- [ ] Public API documented (Doxygen) where the story changed a public interface.
- [ ] Code review approved by at least one reviewer.
- [ ] Merged to the integration branch with CI green.
- [ ] No warnings from new application code.

## Source coverage

| Source location | Topic | Story ids |
|---|---|---|
| §1.1–1.3 | Problem statement and users | E1-001, E2-001, E3-001, E4-001 |
| §2.3 GPIO | Range selection, LED, SPI CS | E1-004, E3-001, E3-002, E3-003 |
| §2.3 UART | BLE module and diagnostic output | E2-001, E2-002, E4-001, E4-002 |
| §2.3 SPI | MCP3204 ADC interface | E1-002 |
| §2.3 Timers | 100 ms sample interval | E5-001, E5-002 |
| §3.1.1 | Resistance measurement | E1-001, E1-002, E1-003, E1-004 |
| §3.1.2 | BLE connectivity | E2-001, E2-002, E2-003 |
| §3.1.3 | 100 ms sampling | E5-001, E5-002 |
| §3.1.4 | JSON data format | E2-005 |
| §3.1.5 | STREAM_OPEN gating | E2-004 |
| §3.1.6 | Status LED | E3-001, E3-002, E3-003 |
| §3.1.7 | Diagnostic output | E4-001, E4-002 |
| §3.2.1 | Graceful disconnect handling | E2-006 |
| §3.2.2 | Overrun protection | E5-003 |
| §3.2.3 | Compile-time verbosity | E4-003 |
| §4.2 | Timing constraints | E5-001, E5-002, E3-002, E3-003 |
| §4.3 | Memory constraints | E6-002 |
| §4.4 | No RTOS, MCC integration, no heap | E6-001, E6-002, E6-003 |
| §5.1–5.12 | Success criteria | All stories |

---

## Backlog

### Epic E1 — Resistance Measurement Acquisition

Acquire accurate resistance values from the Multimeter Click hardware and convert to engineering units.

**Source:** §3.1.1, §2.3 SPI

---

#### E1-001 — Operator views real-time resistance measurements on connected device

> As a Test Engineer,
> I want to view real-time resistance measurements on my connected device,
> so that I can monitor circuit values during automated testing without physical cable connections.

- **Type:** User-facing
- **Layer:** Product
- **Parent:** —
- **Persona:** Test Engineer
- **Priority:** Must
- **Estimate:** Epic — see children for sizing
- **Depends on:** —
- **Status:** Draft
- **Source:** §1.1–1.2, §3.1.1

##### Acceptance criteria

###### Scenario: Valid resistance reading displayed

```gherkin
Given the device is powered on and BLE connected with STREAM_OPEN
When  a 1 kΩ resistor is connected to the measurement terminals
Then  the received JSON data shows an ohm value within ±5% of 1000
And   readings update every 100 ms
```

###### Scenario: Zero resistance reading

```gherkin
Given the device is powered on and BLE connected with STREAM_OPEN
When  the measurement terminals are shorted (0 Ω)
Then  the received JSON data shows an ohm value near 0 (within measurement floor)
```

###### Scenario: Full-scale resistance reading

```gherkin
Given the device is powered on and BLE connected with STREAM_OPEN
When  a 10 kΩ resistor is connected to the measurement terminals
Then  the received JSON data shows an ohm value within ±5% of 10000
```

---

#### E1-002 — Read resistance value from MCP3204 ADC

> As a Firmware Developer,
> I want to read raw ADC values from the MCP3204 via SPI,
> so that I have the raw measurement data needed for resistance calculation.

- **Type:** Enabler
- **Layer:** Module
- **Parent:** E1-001
- **Persona:** Firmware Developer
- **Priority:** Must
- **Estimate:** S (1–2 days)
- **Depends on:** E6-003
- **Status:** Draft
- **Source:** §2.3 SPI, §3.1.1

##### Acceptance criteria

###### Scenario: Successful ADC read from Channel 2

```gherkin
Given the MCP3204 is powered and SPI is initialized
When  a read is requested for Channel 2
Then  a 12-bit value (0–4095) is returned
And   the SPI transaction completes within the sample budget
```

###### Additional rules

- SPI clock rate is 1 MHz, Mode 0 per MCP3204 datasheet.
- Chip select (Multimeter_SPI_CS) is asserted low during transaction.
- Read uses single-ended mode for Channel 2.

---

#### E1-003 — Convert raw ADC counts to ohms

> As a Firmware Developer,
> I want to convert raw ADC counts to resistance in ohms,
> so that measurements are in meaningful engineering units for transmission.

- **Type:** Enabler
- **Layer:** Module
- **Parent:** E1-001
- **Persona:** Firmware Developer
- **Priority:** Must
- **Estimate:** S (1 day)
- **Depends on:** E1-002
- **Status:** Draft
- **Source:** §3.1.1, §5.8

##### Acceptance criteria

###### Scenario: Zero ohm conversion

```gherkin
Given the documented conversion formula
When  the ADC returns the count corresponding to 0 Ω
Then  the converted value is 0 (or within measurement floor)
```

###### Scenario: 10 kΩ conversion

```gherkin
Given the documented conversion formula
When  the ADC returns the count corresponding to 10 kΩ
Then  the converted value is 10000 (±tolerance)
```

###### Additional rules

- Conversion formula must match the documented Multimeter Click formula.
- Integer math preferred to avoid floating-point overhead.

---

#### E1-004 — Configure 0–20 kΩ measurement range

> As a Firmware Developer,
> I want to configure the measurement range to 0–20 kΩ via GPIO,
> so that resistance readings cover the required 0–10 kΩ range with appropriate resolution.

- **Type:** Enabler
- **Layer:** Module
- **Parent:** E1-001
- **Persona:** Firmware Developer
- **Priority:** Must
- **Estimate:** XS (0.5 days)
- **Depends on:** E6-003
- **Status:** Draft
- **Source:** §2.3 GPIO, §3.1.1, §4.4

##### Acceptance criteria

###### Scenario: Range GPIOs configured at startup

```gherkin
Given the device is initializing
When  the measurement module initializes
Then  RES_A, RES_B, RES_C GPIOs are set to select the 0–20 kΩ range
And   the range remains fixed during operation
```

###### Additional rules

- GPIO values per Multimeter Click schematic v102.
- Range is set once at startup; no runtime switching.

---

### Epic E2 — BLE Connectivity and Data Transmission

Establish wireless communication via the RNBD350 BLE module and stream measurement data.

**Source:** §3.1.2, §3.1.4, §3.1.5, §3.2.1

---

#### E2-001 — Operator receives resistance data wirelessly via BLE

> As a Field Technician,
> I want to receive resistance measurements wirelessly on my mobile device,
> so that I can monitor values without being tethered to the measurement equipment.

- **Type:** User-facing
- **Layer:** Product
- **Parent:** —
- **Persona:** Field Technician
- **Priority:** Must
- **Estimate:** Epic — see children for sizing
- **Depends on:** E1-001
- **Status:** Draft
- **Source:** §1.1–1.2, §3.1.2, §3.1.4, §3.1.5

##### Acceptance criteria

###### Scenario: Data streaming begins after STREAM_OPEN

```gherkin
Given the device is powered on and a BLE client connects
When  the RNBD350 reports STREAM_OPEN
Then  JSON measurement records begin transmitting at 100 ms intervals
And   no data is transmitted before STREAM_OPEN (connection alone is insufficient)
```

###### Scenario: JSON format is correct

```gherkin
Given data streaming is active
When  a measurement is transmitted
Then  the format is exactly {"Meter":{"ohm":<value>}}\r\n
And   <value> is the resistance in ohms as an integer or decimal
```

###### Scenario: Disconnect stops transmission immediately

```gherkin
Given data streaming is active
When  the BLE client disconnects
Then  transmission stops immediately
And   any pending unsent data is discarded
```

---

#### E2-002 — Initialize RNBD350 BLE module communication

> As a Firmware Developer,
> I want to initialize UART communication with the RNBD350 module,
> so that the firmware can send commands and receive status messages.

- **Type:** Enabler
- **Layer:** Module
- **Parent:** E2-001
- **Persona:** Firmware Developer
- **Priority:** Must
- **Estimate:** S (1 day)
- **Depends on:** E6-003
- **Status:** Draft
- **Source:** §2.3 UART, §3.1.2

##### Acceptance criteria

###### Scenario: UART configured for RNBD350

```gherkin
Given the device is initializing
When  SERCOM0 is configured
Then  baud rate is 115200, format is 8N1
And   the UART is dedicated to RNBD350 communication only
```

###### Additional rules

- SERCOM0 traffic never mixed with diagnostic output.
- Uses MCC-generated UART driver without modification.

---

#### E2-003 — Parse RNBD350 status messages

> As a Firmware Developer,
> I want to parse status messages from the RNBD350,
> so that the firmware can detect connection state changes and streaming readiness.

- **Type:** Enabler
- **Layer:** Module
- **Parent:** E2-001
- **Persona:** Firmware Developer
- **Priority:** Must
- **Estimate:** M (2–3 days)
- **Depends on:** E2-002
- **Status:** Draft
- **Source:** §3.1.5, §5.6

##### Acceptance criteria

###### Scenario: Byte-at-a-time parsing

```gherkin
Given UART data arrives one byte at a time
When  a complete status message is received
Then  the message is correctly identified
And   partial messages do not trigger false events
```

###### Scenario: Fragmented message handling

```gherkin
Given a status message arrives split across multiple UART reads
When  all fragments are received
Then  the complete message is correctly parsed
```

###### Scenario: Adjacent message handling

```gherkin
Given two status messages arrive in the same UART buffer
When  the buffer is processed
Then  both messages are correctly identified and handled
```

###### Additional rules

- Parser handles: `%CONNECT%`, `%DISCONNECT%`, `%STREAM_OPEN%`.
- Uses only documented RNBD350 status strings per DS50003684B.

---

#### E2-004 — Detect STREAM_OPEN event to enable data transmission

> As a Firmware Developer,
> I want to detect the STREAM_OPEN event from the RNBD350,
> so that data transmission only begins when transparent UART mode is established.

- **Type:** Enabler
- **Layer:** Module
- **Parent:** E2-001
- **Persona:** Firmware Developer
- **Priority:** Must
- **Estimate:** S (1 day)
- **Depends on:** E2-003
- **Status:** Draft
- **Source:** §3.1.5, §5.4

##### Acceptance criteria

###### Scenario: STREAM_OPEN enables transmission

```gherkin
Given a BLE client is connected
When  the RNBD350 sends %STREAM_OPEN%
Then  the streaming_enabled flag is set to true
And   data transmission begins on the next sample
```

###### Scenario: Connection without STREAM_OPEN does not enable transmission

```gherkin
Given the device receives %CONNECT%
When  %STREAM_OPEN% has not been received
Then  streaming_enabled remains false
And   no measurement data is transmitted
```

---

#### E2-005 — Format measurement as JSON and transmit

> As a Firmware Developer,
> I want to format resistance measurements as JSON and transmit via BLE,
> so that receiving applications can parse the data in a standardized format.

- **Type:** Enabler
- **Layer:** Module
- **Parent:** E2-001
- **Persona:** Firmware Developer
- **Priority:** Must
- **Estimate:** S (1–2 days)
- **Depends on:** E1-003, E2-004
- **Status:** Draft
- **Source:** §3.1.4, §5.7

##### Acceptance criteria

###### Scenario: JSON format matches canonical specification

```gherkin
Given a resistance value of 1234 ohms
When  the measurement is formatted for transmission
Then  the output is exactly {"Meter":{"ohm":1234}}\r\n byte-for-byte
```

###### Additional rules

- No whitespace except as shown in canonical format.
- Terminated with `\r\n` (CRLF).
- Uses static buffer; no heap allocation.

---

#### E2-006 — Handle BLE disconnect and stop transmission

> As a Firmware Developer,
> I want the firmware to immediately stop transmission on BLE disconnect,
> so that no data is sent to a disconnected link and pending data is properly discarded.

- **Type:** Enabler
- **Layer:** Module
- **Parent:** E2-001
- **Persona:** Firmware Developer
- **Priority:** Should
- **Estimate:** S (1 day)
- **Depends on:** E2-003
- **Status:** Draft
- **Source:** §3.2.1, §5.5

##### Acceptance criteria

###### Scenario: Disconnect stops transmission immediately

```gherkin
Given data streaming is active
When  the RNBD350 sends %DISCONNECT%
Then  streaming_enabled is set to false
And   any pending data in the transmit buffer is discarded
And   no further transmission attempts occur until next STREAM_OPEN
```

---

### Epic E3 — Visual Status Feedback

Provide visual indication of device connection state via LED.

**Source:** §3.1.6

---

#### E3-001 — Operator sees LED feedback indicating connection state

> As a Field Technician,
> I want to see the LED indicate whether BLE is connected,
> so that I know the device state at a glance without checking my mobile device.

- **Type:** User-facing
- **Layer:** Product
- **Parent:** —
- **Persona:** Field Technician
- **Priority:** Must
- **Estimate:** Epic — see children for sizing
- **Depends on:** —
- **Status:** Draft
- **Source:** §3.1.6, §5.1, §5.2

##### Acceptance criteria

###### Scenario: LED indicates disconnected state

```gherkin
Given the device is powered on
When  no BLE client is connected
Then  the LED toggles at 1 Hz (every 1000 ms)
```

###### Scenario: LED indicates connected state

```gherkin
Given a BLE client is connected with STREAM_OPEN
When  data streaming is active
Then  the LED toggles at 5 Hz (every 200 ms)
```

---

#### E3-002 — Toggle LED at 1 Hz when disconnected

> As a Firmware Developer,
> I want to toggle the LED at 1 Hz when BLE is disconnected,
> so that the operator has visual confirmation the device is powered but not connected.

- **Type:** Enabler
- **Layer:** Module
- **Parent:** E3-001
- **Persona:** Firmware Developer
- **Priority:** Must
- **Estimate:** XS (0.5 days)
- **Depends on:** E6-001
- **Status:** Draft
- **Source:** §3.1.6, §4.2, §5.1

##### Acceptance criteria

###### Scenario: LED toggles every 1000 ms when disconnected

```gherkin
Given streaming_enabled is false
When  the main loop executes
Then  the LED state toggles every 1000 ms (±10 ms jitter acceptable)
```

---

#### E3-003 — Toggle LED at 5 Hz when connected

> As a Firmware Developer,
> I want to toggle the LED at 5 Hz when BLE is connected and streaming,
> so that the operator has visual confirmation of active data transmission.

- **Type:** Enabler
- **Layer:** Module
- **Parent:** E3-001
- **Persona:** Firmware Developer
- **Priority:** Must
- **Estimate:** XS (0.5 days)
- **Depends on:** E2-004, E6-001
- **Status:** Draft
- **Source:** §3.1.6, §4.2, §5.2

##### Acceptance criteria

###### Scenario: LED toggles every 200 ms when connected

```gherkin
Given streaming_enabled is true
When  the main loop executes
Then  the LED state toggles every 200 ms (±10 ms jitter acceptable)
```

---

### Epic E4 — Diagnostic Output

Provide diagnostic information for development and troubleshooting.

**Source:** §3.1.7, §3.2.3

---

#### E4-001 — Developer diagnoses firmware behavior via diagnostic output

> As a Development Engineer,
> I want to view timestamped diagnostic messages on a separate UART,
> so that I can troubleshoot firmware behavior without affecting BLE communication.

- **Type:** User-facing
- **Layer:** Product
- **Parent:** —
- **Persona:** Development Engineer
- **Priority:** Must
- **Estimate:** Epic — see children for sizing
- **Depends on:** —
- **Status:** Draft
- **Source:** §3.1.7, §5.9, §5.10

##### Acceptance criteria

###### Scenario: Diagnostic events logged with metadata

```gherkin
Given a diagnostic event occurs (e.g., BLE connect, sample acquired)
When  the event is logged
Then  the message includes timestamp, severity level, and subsystem identifier
And   the message appears on SERCOM2
```

###### Scenario: Diagnostic output does not affect BLE traffic

```gherkin
Given diagnostic logging is active
When  BLE data transmission occurs simultaneously
Then  SERCOM0 traffic is not corrupted or delayed
And   sampling timing is maintained
```

---

#### E4-002 — Output timestamped diagnostic messages on SERCOM2

> As a Firmware Developer,
> I want to output diagnostic messages with timestamps on SERCOM2,
> so that events can be correlated and timed during debugging.

- **Type:** Enabler
- **Layer:** Module
- **Parent:** E4-001
- **Persona:** Firmware Developer
- **Priority:** Must
- **Estimate:** S (1–2 days)
- **Depends on:** E6-003
- **Status:** Draft
- **Source:** §3.1.7, §5.9

##### Acceptance criteria

###### Scenario: Diagnostic message format

```gherkin
Given a diagnostic event occurs
When  the message is output
Then  it includes: timestamp (ms since boot), severity (DEBUG/INFO/WARN/ERROR), subsystem tag, and message text
```

###### Additional rules

- SERCOM2 configured at 115200 baud, 8N1.
- SERCOM2 used only for diagnostics; never for BLE data.

---

#### E4-003 — Implement compile-time verbosity control

> As a Firmware Developer,
> I want to control diagnostic verbosity at compile time,
> so that I can reduce code size and output noise for production builds.

- **Type:** Enabler
- **Layer:** Module
- **Parent:** E4-001
- **Persona:** Firmware Developer
- **Priority:** Should
- **Estimate:** XS (0.5 days)
- **Depends on:** E4-002
- **Status:** Draft
- **Source:** §3.2.3

##### Acceptance criteria

###### Scenario: Verbosity level controls output

```gherkin
Given the verbosity level is set to INFO at compile time
When  a DEBUG-level message is logged
Then  the message is not output (compiled out or filtered)
And   no runtime overhead for suppressed messages
```

---

### Epic E5 — Timing and Reliability

Ensure consistent sampling and reliable operation under all conditions.

**Source:** §3.1.3, §3.2.2, §4.2

---

#### E5-001 — System operates reliably under timing constraints

> As a Test Engineer,
> I want measurements to arrive at a consistent 100 ms interval,
> so that I can rely on the data rate for time-series analysis.

- **Type:** User-facing
- **Layer:** System
- **Parent:** —
- **Persona:** Test Engineer
- **Priority:** Must
- **Estimate:** Epic — see children for sizing
- **Depends on:** —
- **Status:** Draft
- **Source:** §3.1.3, §4.2

##### Acceptance criteria

###### Scenario: Consistent 100 ms sample interval

```gherkin
Given the device is streaming data
When  100 consecutive samples are captured
Then  the inter-sample interval is 100 ms (±1 ms jitter acceptable)
```

---

#### E5-002 — Sample resistance at fixed 100 ms interval

> As a Firmware Developer,
> I want to trigger resistance sampling every 100 ms,
> so that the system meets the fixed sample rate requirement.

- **Type:** Enabler
- **Layer:** Module
- **Parent:** E5-001
- **Persona:** Firmware Developer
- **Priority:** Must
- **Estimate:** S (1 day)
- **Depends on:** E6-001
- **Status:** Draft
- **Source:** §2.3 Timers, §3.1.3, §4.2

##### Acceptance criteria

###### Scenario: TC0 provides 100 ms tick

```gherkin
Given TC0 is configured for 1 ms tick
When  the software counter reaches 100
Then  a sample is triggered
And   the counter resets for the next interval
```

###### Additional rules

- Uses MCC-generated TC0 driver.
- Software scaling from 1 ms to 100 ms.

---

#### E5-003 — Handle missed samples without data accumulation

> As a Firmware Developer,
> I want to discard stale data if a sample is missed,
> so that the system does not accumulate a backlog of outdated readings.

- **Type:** Enabler
- **Layer:** Module
- **Parent:** E5-001
- **Persona:** Firmware Developer
- **Priority:** Should
- **Estimate:** S (1 day)
- **Depends on:** E5-002
- **Status:** Draft
- **Source:** §3.2.2

##### Acceptance criteria

###### Scenario: Missed sample does not queue stale data

```gherkin
Given a sample interval elapses while the previous sample is still processing
When  the next sample tick occurs
Then  only the most recent measurement is transmitted
And   stale measurements are not queued or transmitted
```

---

### Epic E6 — Architecture and Constraints

Meet system-level architectural constraints for bare-metal embedded operation.

**Source:** §4.3, §4.4

---

#### E6-001 — Bare-metal cooperative main loop architecture

> As a Firmware Developer,
> I want the firmware to use a cooperative main loop without an RTOS,
> so that the system is simple, deterministic, and fits within resource constraints.

- **Type:** Technical
- **Layer:** Component
- **Parent:** —
- **Persona:** Firmware Developer
- **Priority:** Must
- **Estimate:** S (1–2 days)
- **Depends on:** —
- **Status:** Draft
- **Source:** §4.4

##### Acceptance criteria

###### Scenario: Main loop structure

```gherkin
Given the firmware architecture
When  the main loop executes
Then  it polls all subsystems cooperatively (tick, UART RX, state machine)
And   no blocking calls prevent other subsystems from running
And   no RTOS primitives (tasks, semaphores, mutexes) are used
```

---

#### E6-002 — Static memory allocation only

> As a Firmware Developer,
> I want all memory to be statically allocated,
> so that memory usage is predictable and heap fragmentation is impossible.

- **Type:** Technical
- **Layer:** Component
- **Parent:** —
- **Persona:** Firmware Developer
- **Priority:** Must
- **Estimate:** XS (verification)
- **Depends on:** —
- **Status:** Draft
- **Source:** §4.3, §4.4

##### Acceptance criteria

###### Scenario: No heap allocation

```gherkin
Given the complete firmware codebase
When  static analysis or link map is reviewed
Then  no calls to malloc, calloc, realloc, or free exist
And   all buffers are declared with fixed size at compile time
```

---

#### E6-003 — MCC driver integration without modification

> As a Firmware Developer,
> I want to use MCC-generated drivers without modification,
> so that regenerating MCC code does not break the application.

- **Type:** Technical
- **Layer:** Component
- **Parent:** —
- **Persona:** Firmware Developer
- **Priority:** Must
- **Estimate:** XS (verification)
- **Depends on:** —
- **Status:** Draft
- **Source:** §4.4

##### Acceptance criteria

###### Scenario: MCC drivers used as-is

```gherkin
Given the MCC-generated driver files
When  the application interfaces with peripherals
Then  only public MCC APIs are called
And   no MCC-generated source files are modified
```

---

## Assumptions

- **Conversion formula** — story E1-003 assumes the Multimeter Click documentation provides a specific formula for ADC-to-ohms conversion. Confirm formula before implementation.
- **JSON integer format** — story E2-005 assumes resistance values are transmitted as integers. If fractional ohms are needed, format may require decimal representation.
- **STREAM_OPEN as trigger** — stories E2-004, E3-003 assume STREAM_OPEN (not CONNECT) is the correct trigger for enabling data transmission per §3.1.5 and §5.4. This is explicitly stated in the requirements.
- **Diagnostic message format** — story E4-002 assumes a format of `[timestamp] [SEVERITY] [SUBSYSTEM] message`. Exact format to be confirmed.

## Open questions

- **Measurement accuracy tolerance** — blocks **E1-001**, **E1-003**. What is the acceptable tolerance for resistance readings? The AC uses ±5% as a placeholder; confirm with stakeholders.
- **Diagnostic verbosity levels** — blocks **E4-003**. What verbosity levels are needed (DEBUG, INFO, WARN, ERROR)? What is the default for production?
- **STREAM_OPEN timing** — blocks **E2-004**. How long after CONNECT does STREAM_OPEN typically arrive? Is a timeout needed if STREAM_OPEN never arrives?

## Out of scope

- Multi-range support (only 0–20 kΩ) — §7
- Other measurement types (voltage, current, capacitance) — §7
- BLE configuration UI / custom GATT — §7
- OTA firmware update — §7
- Battery monitoring — §7
- Data logging / local storage — §7
- Encryption / custom security — §7
- Custom mobile app — §7
