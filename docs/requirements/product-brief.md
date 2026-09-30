# Product Requirements Brief

## Document Control
- **Project:** Bluetooth Ohmmeter Firmware
- **Version:** 1.0
- **Date:** 2026-09-29
- **Author:** Brian Tate
- **Status:** Draft

---

## 1. Problem Statement

### 1.1 Background
There is a need for a wireless resistance measurement solution that can transmit real-time ohmmeter readings to a PC or mobile device over Bluetooth Low Energy (BLE). This enables remote monitoring of resistance values without physical cable connections, useful for test equipment, field measurements, and automated data logging.

### 1.2 Problem
Existing wired multimeters require direct cable connections for data acquisition, limiting mobility and creating cable management challenges in test setups. A wireless solution that integrates standard measurement hardware with BLE connectivity would improve usability while maintaining measurement accuracy.

### 1.3 Users
- **Test Engineers:** Need to capture resistance measurements for automated test systems
- **Field Technicians:** Require portable resistance measurement with wireless data logging
- **Development Engineers:** Need real-time resistance monitoring during prototype testing

---

## 2. Target Hardware

### 2.1 MCU/Processor
- **Family:** PIC32CM GV (ARM Cortex-M0+)
- **Part Number:** PIC32CM3204GV00064
- **Clock Speed:** 48 MHz (GCLK0)

### 2.2 Memory
- **Flash:** 32 KB
- **RAM:** 4 KB SRAM
- **External:** None

### 2.3 Peripherals Required
- [x] GPIO (RES_A, RES_B, RES_C range selection; LED; Multimeter_SPI_CS)
- [x] UART × 2:
  - SERCOM0: RNBD350 BLE module (115200 baud, 8N1)
  - SERCOM2: Diagnostic output (115200 baud, 8N1)
- [x] SPI:
  - SERCOM5: MCP3204 ADC interface (1 MHz, Mode 0)
- [x] Timers:
  - TC0: 1 ms tick (software-scaled to 100 ms sample interval)
- [ ] ADC: None (external MCP3204 via SPI)
- [ ] DMA: None
- [ ] Other: None

### 2.4 Development Board / Hardware
- **MCU Board:** PIC32CM GV-Value Line Curiosity Nano+ Touch Evaluation Kit
- **Base Board:** Microchip Curiosity Nano Base for Clicks (adapter for Click form factor modules)
- **BLE Module:** Microchip RNBD350PE Bluetooth Low Energy module (Click form factor)
- **Measurement Module:** MIKROE Multimeter Click (MCP3204 ADC, CD4028B range decoder)

---

## 3. Key Features

### 3.1 Must Have (P1)
1. **Resistance Measurement:** Read resistance via MCP3204 ADC Channel 2 with 0–20 kΩ range selection (closest range covering 0–10 kΩ requirement)
2. **BLE Connectivity:** Interface with RNBD350 module for wireless data transmission
3. **100 ms Sampling:** Acquire one resistance measurement every 100 ms
4. **JSON Data Format:** Transmit readings as `{"Meter":{"ohm":<value>}}\r\n`
5. **STREAM_OPEN Gating:** Only transmit data after BLE transparent UART is established (not on connection alone)
6. **Status LED:** Visual feedback of connection state (1 Hz disconnected, 5 Hz connected)
7. **Diagnostic Output:** Timestamped diagnostic messages on dedicated UART (SERCOM2)

### 3.2 Should Have (P2)
1. **Graceful Disconnect Handling:** Immediately stop transmission and discard pending data on disconnect
2. **Overrun Protection:** Handle missed samples without accumulating stale data
3. **Compile-time Verbosity Control:** Adjustable diagnostic output level

### 3.3 Nice to Have (P3)
(None identified in source documents)

---

## 4. Constraints

### 4.1 Power
- **Budget:** No specific budget defined (typical embedded operation)
- **Sleep modes required:** No (continuous operation while powered)

### 4.2 Timing
- **Sample rate:** 100 ms (10 Hz) — fixed, non-negotiable
- **Response time:** Measurements transmitted within same 100 ms window when BLE ready
- **LED toggle (disconnected):** 1,000 ms
- **LED toggle (connected):** 200 ms
- **Boot time:** No specific requirement (reasonable startup acceptable)

### 4.3 Memory
- **Flash usage:** Must fit within MCU flash alongside MCC-generated code
- **RAM usage:** Fixed-size buffers only; no dynamic allocation

### 4.4 Other Constraints
- **No RTOS:** Bare-metal implementation using cooperative main loop
- **MCC Integration:** Must use existing MCC-generated drivers; do not regenerate or modify generated code
- **No Heap Allocation:** All buffers must be statically allocated
- **Single Range:** Fixed 0–20 kΩ range during normal operation
- **BLE Protocol Compliance:** Use only documented RNBD350 commands and status strings
- **UART Isolation:** SERCOM0 for BLE data only; SERCOM2 for diagnostics only; never mix

---

## 5. Success Criteria

The project is successful when:

1. [ ] LED toggles every 1,000 ms while BLE disconnected and no data transmitted
2. [ ] LED toggles every 200 ms while BLE connected and streaming data
3. [ ] One valid JSON measurement record transmitted per 100 ms sample (when connected and STREAM_OPEN)
4. [ ] Data transmission begins only after `%STREAM_OPEN%` received (connection alone insufficient)
5. [ ] Disconnect event immediately stops transmission and discards unsent readings
6. [ ] RNBD350 parser handles byte-at-a-time, fragmented, and adjacent messages correctly
7. [ ] JSON output matches canonical format byte-for-byte: `{"Meter":{"ohm":<value>}}\r\n`
8. [ ] Resistance readings at 0 Ω and 10 kΩ convert correctly per documented formula
9. [ ] All diagnostic events logged to SERCOM2 with timestamp, severity, and subsystem
10. [ ] Diagnostic processing does not corrupt SERCOM0 traffic or delay sampling
11. [ ] Project builds without warnings from new application code
12. [ ] All unit tests pass with ≥95% coverage

---

## 6. Milestones and Timeline

### 6.1 Key Milestones

| Milestone | Target Date | Description |
|-----------|-------------|-------------|
| M1: Requirements Complete | 2026-09-30 | Product brief and user stories approved (G1, G2) |
| M2: Architecture Review | 2026-09-30 | Module decomposition and ADRs approved (G3) |
| M3: Test Plans Complete | 2026-09-30 | Unit test plans for all modules approved (G4) |
| M4: Scaffolding Complete | 2026-09-30 | Compilable stubs verified (G5) |
| M5: TDD Implementation | 2026-09-30 | All modules implemented via TDD (G6) |
| M6: Integration Complete | 2026-09-30 | System integration verified (G7) |
| M7: Release | 2026-09-30 | Final release approved (G8) |

### 6.2 Customer Commitments

| Commitment | Date | Notes |
|------------|------|-------|
| Live Demo | 2026-10-02 | |

### 6.3 External Dependencies

| Dependency | Owner | Due Date | Impact if Missed |
|------------|-------|----------|------------------|
| RNBD350 User Guide DS50003684B | Microchip | Available | Cannot verify command/status strings |
| MCP3204 Datasheet DS21298E | Microchip | Available | Cannot verify SPI protocol |
| Multimeter Click Schematic v102 | MikroElektronika | Available | Cannot verify range GPIO mapping |
| Target hardware availability | TBD | TBD | Cannot perform HIL testing |

---

## 7. Out of Scope

The following are explicitly NOT part of this project:

- **Multi-range support:** Only 0–20 kΩ range; no runtime range switching
- **Other measurement types:** No voltage, current, or capacitance measurement
- **BLE configuration UI:** Module uses default BLE settings; no custom GATT services
- **OTA firmware update:** Not supported in this release
- **Battery monitoring:** Assumes external power; no low-battery detection
- **Data logging/storage:** No local storage; stream-only operation
- **Encryption/security:** Uses default RNBD350 BLE security settings
- **Custom mobile app:** Uses generic BLE terminal or provided PC GUI

---

## 8. References

### 8.1 Authoritative Documents
- RNBD350 Bluetooth Low Energy Module User Guide DS50003684B
- MCP3204 Datasheet DS21298E
- Multimeter Click Schematic v102
- CD4028B Datasheet SCHS033C

### 8.2 Related Documents
- `docs/firmware-design-request.md` — Original design request
- `docs/firmware-implementation-plan.md` — Implementation plan with resolved technical details
- `docs/sdlc/sdlc-manual.md` — Development lifecycle process

---

## Approval

| Role | Name | Date | Signature |
|------|------|------|-----------|
| Project Sponsor | Brian Tate | | |
| Technical Lead | Brian Tate | | |
| Customer Rep | Frank Aloe | | |
