# C4 Diagram Specification

**Project:** Bluetooth Ohmmeter Firmware  
**Version:** 1.0  
**Date:** 2026-09-30  
**Status:** Draft  
**Consumer:** drawio-architect agent

---

## 1. Purpose

This specification defines the elements, relationships, and layout for architecture diagrams following the C4 model. The drawio-architect agent will consume this spec to produce `.drawio` diagram files.

---

## 2. Diagrams Required

| Diagram ID | C4 Level | Title | Purpose |
|------------|----------|-------|---------|
| DIAG-01 | Level 1 (Context) | System Context | Show system boundaries and external actors |
| DIAG-02 | Level 2 (Container) | Container Diagram | Show major runtime containers |
| DIAG-03 | Level 3 (Component) | Component Diagram | Show internal module structure |

---

## 3. DIAG-01: System Context Diagram

### 3.1 Scope

Shows the Bluetooth Ohmmeter system as a single box, with all external actors and systems it interacts with.

### 3.2 Elements

| Element ID | Type | Name | Description |
|------------|------|------|-------------|
| P-01 | Person | Test Engineer | Captures resistance measurements for automated test systems |
| P-02 | Person | Field Technician | Portable resistance measurement with wireless data logging |
| P-03 | Person | Development Engineer | Real-time monitoring during prototype testing |
| S-01 | System | Bluetooth Ohmmeter | Wireless resistance measurement device (THIS SYSTEM) |
| E-01 | External System | BLE Client App | Generic BLE terminal or PC GUI receiving JSON data |
| E-02 | External System | Debug Terminal | Serial terminal for diagnostic output |

### 3.3 Relationships

| From | To | Label | Technology |
|------|----|-------|------------|
| P-01 | S-01 | Views measurements | via E-01 |
| P-02 | S-01 | Views measurements | via E-01 |
| P-03 | S-01 | Views measurements, diagnoses | via E-01, E-02 |
| S-01 | E-01 | Streams JSON measurements | BLE (RNBD350) |
| S-01 | E-02 | Outputs diagnostic logs | UART (115200) |

### 3.4 Layout Hints

- S-01 (Bluetooth Ohmmeter) centered
- Persons (P-01, P-02, P-03) across the top
- External systems (E-01, E-02) at bottom or sides
- Arrows flow from persons down to system, system to externals
- Use standard C4 colors: persons blue, systems gray, this system blue/highlighted

### 3.5 Legend

- Blue rounded rectangle: Person
- Gray rounded rectangle: External System
- Blue box with thick border: System (this system)
- Arrows with labels: Relationships

---

## 4. DIAG-02: Container Diagram

### 4.1 Scope

Shows the internal containers within the Bluetooth Ohmmeter system. For this embedded system, "containers" are the firmware plus the hardware it runs on.

### 4.2 Elements

| Element ID | Type | Name | Technology | Description |
|------------|------|------|------------|-------------|
| C-01 | Container | Firmware Application | C (bare-metal) | Main application code: app + services |
| C-02 | Container | HAL Layer | C | Hardware abstraction interfaces |
| C-03 | Container | MCC Drivers | C (generated) | Microchip Code Configurator drivers |
| C-04 | Container | MCU Hardware | PIC32CM3204 | ARM Cortex-M0+, 48 MHz, 32KB/4KB |
| C-05 | Container | MCP3204 ADC | External IC | 12-bit SPI ADC for resistance measurement |
| C-06 | Container | RNBD350 Module | External IC | Bluetooth Low Energy module |

### 4.3 Relationships

| From | To | Label | Technology |
|------|----|-------|------------|
| C-01 | C-02 | Calls HAL APIs | C function calls |
| C-02 | C-03 | Calls MCC APIs | C function calls |
| C-03 | C-04 | Accesses registers | Memory-mapped I/O |
| C-04 | C-05 | Reads ADC | SPI (1 MHz, SERCOM5) |
| C-04 | C-06 | BLE communication | UART (115200, SERCOM0) |
| C-04 | E-02 | Diagnostic output | UART (115200, SERCOM2) |
| C-06 | E-01 | Streams JSON | BLE transparent UART |

### 4.4 Boundary Groupings

| Group | Contains | Label |
|-------|----------|-------|
| G-01 | C-01, C-02, C-03 | Firmware (32 KB Flash) |
| G-02 | C-04 | MCU (PIC32CM3204GV00064) |
| G-03 | C-05, C-06 | External Peripherals |

### 4.5 Layout Hints

- Vertical stack: C-01 at top, C-02 below, C-03 below that, C-04 at bottom
- C-05 and C-06 to the right of C-04
- E-01 and E-02 outside the system boundary (right side)
- Dashed boundary around entire firmware stack (G-01)
- Solid boundary around MCU (G-02)

### 4.6 Legend

- Blue rounded rectangle: Container (software)
- Gray rounded rectangle: Container (hardware/external)
- Dashed border: Software boundary
- Solid border: Hardware boundary

---

## 5. DIAG-03: Component Diagram

### 5.1 Scope

Shows the internal components (modules) within the Firmware Application container (C-01 + C-02).

### 5.2 Elements

| Element ID | Type | Name | Layer | Description |
|------------|------|------|-------|-------------|
| M-01 | Component | main | Application | Super-loop entry, initialization |
| M-02 | Component | app_state | Application | Connection state machine |
| M-03 | Component | measurement_svc | Service | ADC read + conversion |
| M-04 | Component | ble_svc | Service | RNBD350 parser + TX |
| M-05 | Component | led_svc | Service | LED timing logic |
| M-06 | Component | diag_svc | Service | Diagnostic logging |
| M-07 | Component | hal_gpio | HAL | GPIO abstraction |
| M-08 | Component | hal_spi | HAL | SPI abstraction |
| M-09 | Component | hal_uart | HAL | UART abstraction |
| M-10 | Component | hal_tick | HAL | System tick abstraction |

### 5.3 Relationships

| From | To | Label |
|------|-----|-------|
| M-01 | M-02 | queries/updates state |
| M-01 | M-03 | triggers sample |
| M-01 | M-04 | polls events, transmits |
| M-01 | M-05 | updates LED |
| M-01 | M-06 | logs events |
| M-01 | M-10 | checks tick |
| M-03 | M-07 | controls range/CS |
| M-03 | M-08 | reads ADC |
| M-04 | M-09 | UART BLE |
| M-05 | M-02 | queries state |
| M-05 | M-07 | toggles LED |
| M-06 | M-09 | UART diag |
| M-06 | M-10 | gets timestamp |

### 5.4 Boundary Groupings

| Group | Contains | Label | Color |
|-------|----------|-------|-------|
| L-APP | M-01, M-02 | Application Layer | Light blue |
| L-SVC | M-03, M-04, M-05, M-06 | Service Layer | Light green |
| L-HAL | M-07, M-08, M-09, M-10 | HAL Layer | Light yellow |

### 5.5 Layout Hints

- Horizontal layers stacked vertically
- L-APP at top (1 row with M-01 and M-02 side by side)
- L-SVC below (1 row with M-03, M-04, M-05, M-06)
- L-HAL at bottom (1 row with M-07, M-08, M-09, M-10)
- Arrows flow downward (application -> service -> HAL)
- Exception arrow M-05 -> M-02 shown crossing layers (dashed style)
- Each layer in a colored swimlane/boundary box

### 5.6 Legend

- Blue box: Application layer component
- Green box: Service layer component
- Yellow box: HAL layer component
- Solid arrow: Standard dependency
- Dashed arrow: Cross-layer read-only dependency (exception E-01)

---

## 6. Styling Conventions

### 6.1 Colors

| Element Type | Fill Color | Border Color | Text Color |
|--------------|------------|--------------|------------|
| Person | #08427B | #073B6F | White |
| System (this) | #1168BD | #0E5A9D | White |
| External System | #999999 | #8A8A8A | White |
| Container (SW) | #438DD5 | #3C7FC0 | White |
| Container (HW) | #85BBF0 | #78A8D8 | Black |
| Component (App) | #85BBF0 | #78A8D8 | Black |
| Component (Svc) | #A4D694 | #93C085 | Black |
| Component (HAL) | #FFE6A0 | #E6CF91 | Black |
| Boundary | transparent | #CCCCCC dashed | Gray |

### 6.2 Typography

- Element names: Bold, 12pt
- Descriptions: Regular, 10pt
- Relationship labels: Italic, 9pt
- Technology labels: Regular, 8pt, gray

### 6.3 Arrow Styles

| Type | Style |
|------|-------|
| Standard dependency | Solid, black, arrow at target |
| Read-only/exception | Dashed, gray, arrow at target |
| Data flow | Solid, with label near middle |

---

## 7. File Output

| Diagram ID | Output File | Format |
|------------|-------------|--------|
| DIAG-01 | `docs/architecture/diagrams/context.drawio` | draw.io XML |
| DIAG-02 | `docs/architecture/diagrams/container.drawio` | draw.io XML |
| DIAG-03 | `docs/architecture/diagrams/component.drawio` | draw.io XML |

---

## 8. Verification Checklist (for drawio-architect)

- [ ] All elements from spec present in diagram
- [ ] All relationships from spec present with correct labels
- [ ] Boundary groupings applied
- [ ] Colors match styling conventions
- [ ] Layout follows hints (adjustments allowed for clarity)
- [ ] Legend included in each diagram
- [ ] Files saved to specified paths
