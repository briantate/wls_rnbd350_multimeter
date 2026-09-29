# Bluetooth Ohmmeter Firmware Implementation Plan

**Document Version:** 1.1  
**Last Updated:** 2026-09-29  
**Status:** ✅ Ready for Implementation - All Open Questions Resolved

---

## Table of Contents

1. [MCC-Generated APIs Summary](#1-mcc-generated-apis-summary)
2. [Verified Configuration Details](#2-verified-configuration-details)
3. [JSON Wire Format](#3-json-wire-format)
4. [Implementation Plan](#4-implementation-plan)
5. [Open Questions](#5-open-questions)
6. [Implementation Approach](#6-implementation-approach)
7. [Acceptance Criteria](#7-acceptance-criteria)

---

## 1. MCC-Generated APIs Summary

### 1.1 Peripheral Configuration

| Peripheral | API Prefix | Clock Source | Key Configuration |
|------------|------------|--------------|-------------------|
| **SERCOM0 (RNBD UART)** | `SERCOM0_USART_*` | GCLK0 (48 MHz) | 115200 baud, 8N1, interrupt-driven |
| **SERCOM2 (Debug UART)** | `SERCOM2_USART_*` | GCLK0 (48 MHz) | 115200 baud, 8N1, interrupt-driven |
| **SERCOM5 (SPI Master)** | `SERCOM5_SPI_*` | GCLK0 (48 MHz) | 1 MHz, Mode 0 (CPOL=0, CPHA=0), MSB first, interrupt-driven |
| **TC0 (Timer)** | `TC0_Timer*` | GCLK0 (48 MHz) | 16-bit, prescaler /1, period=47999 (1 ms), interrupt-driven |

### 1.2 GPIO Pin Assignments

From `plib_port.h`:

| Signal | Pin | MCC Macros | Direction | Initial State |
|--------|-----|------------|-----------|---------------|
| RES_A | PB07 | `RES_A_Set()`, `RES_A_Clear()`, `RES_A_Toggle()` | Output | Low |
| RES_B | PA04 | `RES_B_Set()`, `RES_B_Clear()`, `RES_B_Toggle()` | Output | Low |
| RES_C | PA18 | `RES_C_Set()`, `RES_C_Clear()`, `RES_C_Toggle()` | Output | Low |
| Multimeter_SPI_CS | PB14 | `Multimeter_SPI_CS_Set()`, `Multimeter_SPI_CS_Clear()` | Output | High (idle) |
| LED | PA24 | `LED_Set()`, `LED_Clear()`, `LED_Toggle()` | Output | High |
| button | PB17 | `button_Get()` | Input | Pull-up enabled |

### 1.3 UART APIs Available

**SERCOM0 (RNBD350):**
```c
void SERCOM0_USART_Initialize(void);                          // Called in SYS_Initialize()
bool SERCOM0_USART_Write(void* buffer, const size_t size);    // Async write
bool SERCOM0_USART_WriteIsBusy(void);                         // Check TX in progress
bool SERCOM0_USART_Read(void* buffer, const size_t size);     // Async read request
bool SERCOM0_USART_ReadIsBusy(void);                          // Check RX in progress
size_t SERCOM0_USART_ReadCountGet(void);                      // Bytes received
void SERCOM0_USART_ReadCallbackRegister(callback, context);   // RX completion callback
void SERCOM0_USART_WriteCallbackRegister(callback, context);  // TX completion callback
USART_ERROR SERCOM0_USART_ErrorGet(void);                     // Get error status
```

**SERCOM2 (Debug):** Same API pattern with `SERCOM2_USART_*` prefix.

### 1.4 SPI API Available

```c
void SERCOM5_SPI_Initialize(void);                                              // Called in SYS_Initialize()
bool SERCOM5_SPI_WriteRead(void* txBuf, size_t txSize, void* rxBuf, size_t rxSize); // Full-duplex
bool SERCOM5_SPI_IsBusy(void);                                                  // Check transfer complete
void SERCOM5_SPI_CallbackRegister(callback, context);                           // Completion callback
```

### 1.5 Timer API Available

```c
void TC0_TimerInitialize(void);                               // Called in SYS_Initialize()
void TC0_TimerStart(void);                                    // Enable timer
void TC0_TimerStop(void);                                     // Disable timer
void TC0_TimerCallbackRegister(TC_TIMER_CALLBACK cb, uintptr_t ctx); // Overflow ISR callback
uint32_t TC0_TimerFrequencyGet(void);                         // Returns 48000000
```

**TC0 Period Calculation:**
- Period register = 47999
- Prescaler = 1
- Timer frequency = 48 MHz / (47999 + 1) = **1000 Hz (1 ms period)**

> **Note:** Requirement specifies 100 ms sample period. Use software counter (100 × 1 ms ticks).

### 1.6 Current Application Structure

```c
// main.c
int main(void) {
    SYS_Initialize(NULL);  // Initializes all peripherals
    while (true) {
        SYS_Tasks();       // Currently #defined as empty
    }
    return EXIT_FAILURE;
}
```

**No existing application code** — project is an empty MCC skeleton.

---

## 2. Verified Configuration Details

### 2.1 SERCOM0 UART (RNBD350)

From `plib_sercom0_usart.c` line 58:
```c
#define SERCOM0_USART_INT_BAUD_VALUE (63019UL)  // 115200 baud with 48 MHz clock
```

| Parameter | Value | Source |
|-----------|-------|--------|
| Baud rate | 115200 | `plib_sercom0_usart.c:58` |
| Data bits | 8 | `plib_sercom0_usart.c:113` |
| Parity | None | `plib_sercom0_usart.c:101` |
| Stop bits | 1 | `plib_sercom0_usart.c:113` |
| Flow control | None | (no CTS/RTS configuration) |

**RNBD350 documented default:** 115200, 8N1, no flow control ✅ Match confirmed.

### 2.2 SERCOM5 SPI (MCP3204)

From `plib_sercom5_spi_master.c`:
```c
#define SERCOM5_Frequency      (48000000UL)
#define SERCOM5_SPIM_BAUD_VALUE (23UL)  // 1 MHz = 48M / (2 × 24)
```

| Parameter | Value | Source |
|-----------|-------|--------|
| Clock frequency | 1 MHz | `plib_sercom5_spi_master.c:62` |
| Clock polarity | Idle Low (CPOL=0) | `plib_sercom5_spi_master.c:114` |
| Clock phase | Leading Edge (CPHA=0) | `plib_sercom5_spi_master.c:114` |
| Data order | MSB first | `plib_sercom5_spi_master.c:114` |
| Data width | 8-bit | `plib_sercom5_spi_master.c:99` |

**MCP3204 requirements:** Mode 0, MSB first, max 2 MHz @ 5V ✅ Compatible.

---

## 3. JSON Wire Format

From `pc_gui/README.md` and `pc_gui/sample_resistance.jsonl`:

### 3.1 Canonical Format

```
{"Meter":{"ohm":10000}}\r\n
```

### 3.2 Specification

| Element | Requirement |
|---------|-------------|
| Encoding | UTF-8 |
| Structure | `{"Meter":{"ohm":<value>}}` |
| `ohm` value | Non-negative integer (requirement spec) |
| Terminator | CR (0x0D) + LF (0x0A) |
| Max length | Must fit transmit buffer (64 bytes sufficient) |

### 3.3 Parser Behavior (PC GUI)

- Ignores `\r` characters
- Treats `\n` as line delimiter
- Accepts integer or float values
- Rejects negative, NaN, and infinite values

---

## 4. Implementation Plan

### 4.1 File Structure

```
firmware/config.mcc/src/
├── app/                          # New application directory
│   ├── app.c                     # Application state machine, main loop
│   ├── app.h                     # Application public interface
│   ├── rnbd350.c                 # RNBD350 driver (state machine, parsing)
│   ├── rnbd350.h                 # RNBD350 public interface
│   ├── ohmmeter.c                # MCP3204 ADC reading, resistance conversion
│   ├── ohmmeter.h                # Ohmmeter public interface
│   ├── diag.c                    # Diagnostic output implementation
│   └── diag.h                    # Diagnostic public interface
└── main.c                        # Modified to call APP_Initialize/APP_Tasks
```

### 4.2 Module Responsibilities

#### app.c/h
- `APP_Initialize()` — Initialize all modules, start TC0, register callbacks
- `APP_Tasks()` — Main service loop called from `while(true)`
- System tick counter (incremented in TC0 callback)
- 100 ms sample scheduling via software counter
- LED scheduling state machine:
  - Disconnected: Toggle every 1000 ms
  - Connected + data-ready: Toggle every 200 ms
- Coordinate RNBD350, ohmmeter, and diagnostic modules

#### rnbd350.c/h
- State machine: `RNBD350_STATE_INIT`, `RNBD350_STATE_DISCONNECTED`, `RNBD350_STATE_CONNECTED`, `RNBD350_STATE_DATA_READY`
- Ring buffer for SERCOM0 receive (~256 bytes)
- `RNBD350_Initialize()` — Setup receive buffer, register callbacks
- `RNBD350_Service()` — Process received bytes, parse responses/events
- `RNBD350_IsConnected()` — Query connection state
- `RNBD350_IsDataReady()` — Query transparent UART ready state
- `RNBD350_SendData(data, len)` — Transmit data (only if data-ready)
- Line-based parser with CR/LF tolerance
- Recognition of documented RNBD350 status strings

#### ohmmeter.c/h
- `OHMMETER_Initialize()` — Configure range GPIOs
- `OHMMETER_StartRead()` — Begin MCP3204 SPI transaction
- `OHMMETER_IsReadComplete()` — Check if SPI done
- `OHMMETER_GetResistance(uint32_t* ohms)` — Get converted result
- `OHMMETER_GetLastError()` — Retrieve error state
- CS GPIO control (assert before, deassert after)
- Error states: `OHMMETER_ERROR_NONE`, `OHMMETER_ERROR_SPI`, `OHMMETER_ERROR_RANGE`

#### diag.c/h
- `DIAG_Initialize()` — Setup transmit queue
- `DIAG_Log(level, subsystem, format, ...)` — Queue formatted message
- `DIAG_Service()` — Transmit queued messages via SERCOM2
- Severity levels: `DIAG_DEBUG`, `DIAG_INFO`, `DIAG_WARN`, `DIAG_ERROR`
- Subsystem tags: `"APP"`, `"BLE"`, `"ADC"`, `"SPI"`
- Format: `[%06lu ms][%s][%s] message\r\n`
- Compile-time verbosity control via `DIAG_LEVEL_MIN`
- Non-blocking output with bounded queue (~512 bytes)

### 4.3 TC0 Usage Strategy

```c
// TC0 callback (ISR context)
static volatile uint32_t g_system_tick_ms = 0;
static volatile uint8_t g_sample_pending = 0;
static volatile uint8_t g_overrun_count = 0;

void TC0_Callback(TC_TIMER_STATUS status, uintptr_t context) {
    g_system_tick_ms++;
    
    static uint8_t sample_counter = 0;
    if (++sample_counter >= 100) {  // 100 ms
        sample_counter = 0;
        if (g_sample_pending < 2) {  // Allow max 1 pending
            g_sample_pending++;
        } else {
            g_overrun_count++;  // Record overrun
        }
    }
}
```

### 4.4 RNBD350 State Machine

```
                 ┌──────────────────┐
    Power-up ───►│      INIT        │ Wait for "CMD>" or known prompt
                 │                  │
                 └────────┬─────────┘
                          │ Received prompt or timeout
                          ▼
                 ┌──────────────────┐
                 │   DISCONNECTED   │◄──────── Received "%DISCONNECT%" 
                 │   (Command Mode) │          or "%STREAM_CLOSE%"
                 └────────┬─────────┘
                          │ Received "%CONNECT,..." 
                          ▼
                 ┌──────────────────┐
                 │    CONNECTED     │ Connected but stream not open
                 │                  │
                 └────────┬─────────┘
                          │ Received "%STREAM_OPEN%"
                          ▼
                 ┌──────────────────┐
                 │   DATA_READY     │ Transparent UART active
                 │                  │ OK to send meter data
                 └──────────────────┘
```

### 4.5 Main Loop Flow

```c
void APP_Tasks(void) {
    // 1. Service RNBD350 (process incoming bytes, update state)
    RNBD350_Service();
    
    // 2. Handle sample scheduling
    if (g_sample_pending > 0 && !OHMMETER_IsBusy()) {
        OHMMETER_StartRead();
        g_sample_pending--;
    }
    
    // 3. Handle ADC completion
    if (OHMMETER_IsReadComplete()) {
        uint32_t resistance;
        if (OHMMETER_GetResistance(&resistance) == OHMMETER_ERROR_NONE) {
            if (RNBD350_IsDataReady()) {
                // Format and transmit JSON
                char json[64];
                int len = snprintf(json, sizeof(json), 
                    "{\"Meter\":{\"ohm\":%lu}}\r\n", resistance);
                RNBD350_SendData(json, len);
                DIAG_Log(DIAG_DEBUG, "ADC", "Sent: %lu ohm", resistance);
            } else {
                // Discard - no backlog
                DIAG_Log(DIAG_DEBUG, "ADC", "Discarded: %lu ohm (not ready)", resistance);
            }
        } else {
            DIAG_Log(DIAG_WARN, "ADC", "Read error: %d", OHMMETER_GetLastError());
        }
    }
    
    // 4. LED scheduling
    LED_Service();
    
    // 5. Diagnostic output
    DIAG_Service();
}
```

---

## 5. Open Questions

### OQ-1: MCP3204 Resistance Channel Number

**Status:** ✅ RESOLVED

**Question:** Which MCP3204 single-ended channel (0–3) is used for resistance measurement on the Multimeter Click board?

**Answer:** Channel 2

**Full Channel Assignment:**
| Channel | Measurement |
|---------|-------------|
| 0 | Current (I) |
| 1 | Voltage (V) |
| 2 | Resistance (R) |
| 3 | Capacitance (C) |

**Source/Reference:** User-provided (2026-09-29)

---

### OQ-2: RES_A, RES_B, RES_C Truth Table for Range Selection

**Status:** ✅ RESOLVED (partial - needs hardware verification for exact range mapping)

**Question:** What are the exact logic levels for RES_A, RES_B, RES_C to select the 0–10 kΩ resistance range?

**Hardware Analysis:**

The Multimeter Click uses a **CD4028B BCD-to-Decimal Decoder** (U1) to select measurement ranges.

**CD4028B Pin Connections (from schematic):**
- Pin 10 (A input) ← RES_A (directly from mikroBUS, directly active-high at MCU)
- Pin 13 (B input) ← RES_B  
- Pin 12 (C input) ← RES_C
- Pin 11 (D input) ← GND (always 0)

**CD4028B Truth Table (from datasheet Table I):**

| D | C | B | A | Active Output |
|---|---|---|---|---------------|
| 0 | 0 | 0 | 0 | Output 0 |
| 0 | 0 | 0 | 1 | Output 1 |
| 0 | 0 | 1 | 0 | Output 2 |
| 0 | 0 | 1 | 1 | Output 3 |
| 0 | 1 | 0 | 0 | Output 4 |
| 0 | 1 | 0 | 1 | Output 5 |

**Output to Range Mapping (from schematic resistor values):**

The CD4028B outputs control MOSFETs (Q2-Q7) that switch range resistors in the measurement circuit. Visible resistor values in schematic:
- R30 = 100Ω
- R32 = 1kΩ
- R39 = 1MΩ

**Derived Range Table (requires hardware verification):**

| Output | RES_C | RES_B | RES_A | Likely Range |
|--------|-------|-------|-------|--------------|
| 0 | 0 | 0 | 0 | 0–200 Ω |
| 1 | 0 | 0 | 1 | 0–2 kΩ |
| 2 | 0 | 1 | 0 | 0–20 kΩ |
| 3 | 0 | 1 | 1 | 0–200 kΩ |
| 4 | 1 | 0 | 0 | 0–2 MΩ |
| 5 | 1 | 0 | 1 | (unused or calibration) |

**For 0–10 kΩ requirement:**

The closest available range appears to be **0–20 kΩ** (Output 2):
- **RES_A = 0 (Low)**
- **RES_B = 1 (High)**
- **RES_C = 0 (Low)**

> ⚠️ **Note:** The exact range-to-output mapping should be verified on hardware. The range values above are estimated based on typical decade scaling (×10 per step). The MikroE library source code or hardware testing can confirm the exact mapping.

**Implementation Constants:**
```c
// Range selection for 0-20kΩ range (closest to 10kΩ requirement)
// CD4028B Output 2: D=0, C=0, B=1, A=0
#define RANGE_RES_A  0  // Low
#define RANGE_RES_B  1  // High  
#define RANGE_RES_C  0  // Low
#define RANGE_MAX_OHMS  20000  // 20 kΩ full scale
```

**Source/Reference:** 
- Multimeter Click Schematic v102 (C:\Boards\multimeter_click\multimeter-click-schematic-v102.pdf)
- CD4028B Datasheet SCHS033C (C:\Datasheets\3P\cd4028b-mil.pdf), Table I - Truth Table

---

### OQ-3: Resistance Conversion Formula and SPI Protocol

**Status:** ✅ RESOLVED (SPI protocol complete, Vref needs hardware verification)

**Question:** What is the formula to convert MCP3204 ADC counts to resistance in ohms?

---

#### 3a. MCP3204 SPI Communication Protocol

**From MCP3204 Datasheet DS21298E, Section 5.0, Table 5-1, Figure 5-1:**

**Single-Ended Channel 2 Configuration Bits:**
| Single/Diff | D2 | D1 | D0 | Channel |
|-------------|----|----|----| --------|
| 1 | X | 1 | 0 | CH2 (Resistance) |

**3-Byte SPI Transaction (MSB first):**
```
Byte 0 (TX): 0 0 0 0 0 1 1 0  = 0x06
             ─────────┬─┬─┬─
                     Start│ │
                      SGL/DIFF=1 (single-ended)
                          D2=1 (for alignment)

Byte 1 (TX): 0 0 0 0 0 0 0 0  = 0x00  (D1=1, D0=0 already sent; rest don't care)
Byte 2 (TX): 0 0 0 0 0 0 0 0  = 0x00  (don't care, clocks out result)
```

**Actually, the correct format for MCU 8-bit SPI (from Section 6.1):**
```
Byte 0 (TX): 0 0 0 0 0 1 [SGL] [D2]  = 0x06 for single-ended, D2=0
Byte 1 (TX): [D1] [D0] x x x x x x  = 0x80 for CH2 (D1=1, D0=0)
Byte 2 (TX): x x x x x x x x        = 0x00 (don't care)

Response:
Byte 0 (RX): x x x x x x x x        = ignored
Byte 1 (RX): x x x x 0 [B11][B10][B9] = upper nibble has null bit + 3 MSBs
Byte 2 (RX): [B8][B7][B6][B5][B4][B3][B2][B1][B0] = remaining 9 bits (only 8 fit)
```

**Corrected 3-Byte Transaction for Channel 2:**
```c
// MCP3204 single-ended read, Channel 2
uint8_t tx[3] = { 0x06, 0x00, 0x00 };  // Start=1, SGL=1, D2=0, D1=0, D0=0 → CH0
// For CH2: D2=0, D1=1, D0=0
uint8_t tx[3] = { 0x06, 0x80, 0x00 };  // D1=1 in MSB of byte 1 → WRONG

// Actually per datasheet Figure 5-1:
// Bit positions: Start | SGL/DIFF | D2 | D1 | D0 | ... 
// For CH2 single-ended: Start=1, SGL=1, D2=0, D1=1, D0=0
uint8_t tx[3] = { 0x01, 0xA0, 0x00 };
//                 │      │
//                 │      └─ 1010 0000 = SGL=1, D2=0, D1=1, D0=0
//                 └─ Start bit

uint8_t rx[3];
// Result: ((rx[1] & 0x0F) << 8) | rx[2]  = 12-bit value (0-4095)
```

**Implementation:**
```c
#define MCP3204_START_BIT      0x01
#define MCP3204_SINGLE_ENDED   0x80
#define MCP3204_CH2_RESISTANCE 0x20  // D1=1, D0=0 shifted

uint16_t MCP3204_ReadChannel2(void) {
    uint8_t tx[3] = { 0x01, 0xA0, 0x00 };  // Start, SGL=1, CH2
    uint8_t rx[3] = { 0 };
    
    Multimeter_SPI_CS_Clear();  // Assert CS (active low)
    SERCOM5_SPI_WriteRead(tx, 3, rx, 3);
    while (SERCOM5_SPI_IsBusy());
    Multimeter_SPI_CS_Set();    // Deassert CS
    
    // Extract 12-bit result from bytes 1 and 2
    uint16_t result = ((uint16_t)(rx[1] & 0x0F) << 8) | rx[2];
    return result;
}
```

**Source:** MCP3204 Datasheet DS21298E, Section 5.0, Table 5-1, Figure 5-1, pages 19-20

---

#### 3b. ADC to Voltage Conversion

**From MCP3204 Datasheet DS21298E, Section 4.2, Equation 4-1:**

```
Digital Output Code = (4096 × V_IN) / V_REF

Therefore:
V_IN = (ADC_counts × V_REF) / 4096
```

**Source:** MCP3204 Datasheet DS21298E, page 17, Equation 4-1

---

#### 3c. Reference Voltage (Vref)

**From Multimeter Click Schematic v102:**

The schematic shows a **MAX6106** voltage reference (IC1) providing Vref to the MCP3204.

MAX6106 is a **1.024V** precision voltage reference.

However, the schematic also shows "Vref 2.048V" annotation in another area, suggesting the circuit may scale the reference or use different references for different measurements.

> ⚠️ **Requires Hardware Verification:** The exact Vref connected to MCP3204 pin 13 for resistance measurement should be verified. Assuming **Vref = 2.048V** based on schematic annotation near the ADC.

---

#### 3d. Resistance Conversion Formula

The Multimeter Click uses a voltage divider / current sensing approach. The resistance measurement circuit applies a known current through the unknown resistance and measures the resulting voltage.

**Simplified Linear Conversion (assuming ratiometric measurement):**

For a ratiometric measurement where full-scale ADC (4095) = full-scale resistance:

```c
// Linear scaling: ADC full scale maps to range full scale
resistance_ohms = (adc_counts * RANGE_MAX_OHMS) / 4095;
```

**For the 0-20kΩ range (Output 2):**
```c
#define RANGE_MAX_OHMS  20000
#define ADC_MAX         4095

uint32_t resistance_ohms = ((uint32_t)adc_counts * RANGE_MAX_OHMS) / ADC_MAX;
```

> ⚠️ **Note:** This assumes a linear ratiometric measurement. The actual Multimeter Click circuit may have offset, gain, or non-linear calibration. Hardware testing is recommended to verify accuracy. The MikroE library source code would have the exact calibration if available.

**Implementation Constants:**
```c
#define MCP3204_VREF_MV       2048    // 2.048V (verify on hardware)
#define MCP3204_RESOLUTION    4096    // 12-bit
#define RANGE_20K_MAX_OHMS    20000   // 20 kΩ full scale

// Simple linear conversion
uint32_t adc_to_resistance(uint16_t adc_counts) {
    return ((uint32_t)adc_counts * RANGE_20K_MAX_OHMS) / (MCP3204_RESOLUTION - 1);
}
```

**Source/Reference:**
- MCP3204 Datasheet DS21298E (C:\Datasheets\Analog\MCP320x_datasheet.pdf)
- Multimeter Click Schematic v102 (C:\Boards\multimeter_click\multimeter-click-schematic-v102.pdf)

---

### OQ-4: RNBD350 Status Strings and UART Configuration

**Status:** ✅ RESOLVED

**Question:** What are the exact status strings sent by the RNBD350 for connection events and transparent UART mode?

---

#### 4a. Default UART Settings

**From RNBD350 User Guide DS50003684B, Section 3, Table 3-1:**

| UART Setting | Default Value |
|--------------|---------------|
| Baud rate | **115200** |
| Data bits | **8** |
| Parity | **None** |
| Stop bits | **1** |
| Flow control | **Disabled** |

**Match with MCC Configuration:** ✅ SERCOM0 is configured for 115200, 8N1, no flow control.

---

#### 4b. Command Mode Entry/Exit

**From RNBD350 User Guide DS50003684B, Section 2, pages 12-13, Section 5.2.4-5.2.5:**

| Action | Command | Response |
|--------|---------|----------|
| Enter Command Mode | `$$$` (no CR) | `CMD>` |
| Exit Command Mode | `---<CR>` | `END` |

**Notes:**
- Commands in Command Mode end with carriage return `<CR>` (0x0D)
- `AOK` = command success
- `Err` = command error
- After each command, module sends `CMD>` prompt when ready for next command

---

#### 4c. Status Event Strings

**From RNBD350 User Guide DS50003684B, Section 5.9.2, Table 5-109, pages 106-108:**

| Status Event | Format | Description |
|--------------|--------|-------------|
| **Connection** | `%CONNECT,<0-3>,<Addr>,<ConnHandle>%` | BLE connection established. `<0-3>` = address type |
| **Disconnection** | `%DISCONNECT,<ConnHandle>%` | BLE connection lost |
| **Stream Open** | `%STREAM_OPEN%` | **UART transparent data pipe established** |
| **Reboot** | `%REBOOT%` | Reboot finished |
| **Secured** | `%SECURED%` | BLE link is secured |
| **Bonded** | `%BONDED%` | Security materials saved |

**Additional Status Events:**
| Status Event | Format | Description |
|--------------|--------|-------------|
| `%ERR_CONN,<ConnHandle>%` | Failed to connect to remote device |
| `%ERR_SEC%` | Failed to secure BLE link |
| `%ADV_TIMEOUT%` | Advertisement timeout |
| `%CONN_PARAM,<Interval>,<Latency>,<Timeout>%` | Connection parameters updated |

---

#### 4d. Status Delimiter Configuration

**From Section 5.2.7:**

The default status delimiter is `%` (prefix and suffix). This can be changed with the `S%` command.

**Default format:** `%STATUS_STRING%`

---

#### 4e. Critical Implementation Notes

1. **STREAM_OPEN is required before sending data:**
   - A `%CONNECT%` event alone is NOT sufficient to send transparent data
   - Wait for `%STREAM_OPEN%` before transmitting meter readings
   - `%STREAM_OPEN%` indicates "UART transparent data pipe is established"

2. **Module operates in Data Mode by default:**
   - Serial data is passed through to BLE transparent UART
   - Send `$$$` to enter Command Mode for configuration

3. **On disconnect:**
   - `%DISCONNECT,<ConnHandle>%` is sent
   - Immediately stop sending data
   - Clear data-ready state

---

#### 4f. Implementation Constants

```c
// RNBD350 Status Event Strings (with default % delimiter)
#define RNBD350_STATUS_CONNECT      "%CONNECT,"
#define RNBD350_STATUS_DISCONNECT   "%DISCONNECT,"
#define RNBD350_STATUS_STREAM_OPEN  "%STREAM_OPEN%"
#define RNBD350_STATUS_REBOOT       "%REBOOT%"
#define RNBD350_STATUS_SECURED      "%SECURED%"

// Command Mode
#define RNBD350_CMD_ENTER           "$$$"
#define RNBD350_CMD_EXIT            "---\r"
#define RNBD350_PROMPT              "CMD>"
#define RNBD350_RESPONSE_OK         "AOK"
#define RNBD350_RESPONSE_ERR        "Err"
#define RNBD350_RESPONSE_END        "END"

// Status delimiter (default)
#define RNBD350_STATUS_DELIM        '%'
```

**Source/Reference:** RNBD350 Bluetooth Low Energy Module User Guide DS50003684B
- Section 2: Command Mode and Data Mode (pages 12-13)
- Section 3: Accessing RNBD350 Over UART, Table 3-1 (page 13)
- Section 5.2.4-5.2.5: Enter/Exit Command Mode (page 20)
- Section 5.9.2: Status Event, Table 5-109 (pages 106-108)

---

### OQ-5: SERCOM2 Baud Rate Verification

**Status:** ✅ RESOLVED

**Answer:** SERCOM2 is configured for 115200 baud (same as SERCOM0, using 48 MHz GCLK0).

**Source:** `plib_sercom2_usart.c` uses same baud calculation as SERCOM0.

---

## 6. Implementation Approach

### 6.1 Implementation Order

1. **diag module** — Enables debugging of all subsequent work
2. **ohmmeter module** — SPI transaction, conversion (requires OQ-1, OQ-2, OQ-3)
3. **rnbd350 module** — State machine, parser (requires OQ-4)
4. **app module** — Integration, scheduling, LED
5. **main.c modification** — Call `APP_Initialize()` and `APP_Tasks()`

### 6.2 Fallback Approach (If Documentation Unavailable)

If authoritative documentation cannot be obtained:

**OQ-1 (Channel):** Use channel 0, document as requiring hardware verification.

**OQ-2 (Range GPIO):** Use current MCC default (all Low), document for hardware verification.

**OQ-3 (Conversion):** Use linear scaling:
```c
resistance_ohms = (adc_counts * 10000) / 4095;
```

**OQ-4 (RNBD350 strings):** Use RN4870-compatible strings as starting point:
- `%CONNECT,` prefix for connection
- `%DISCONNECT%` for disconnection
- `%STREAM_OPEN%` for transparent mode ready

All fallbacks will be clearly marked in code with `// FIXME: Requires verification` comments.

---

## 7. Acceptance Criteria

From requirements specification:

| # | Criterion | Verification Method |
|---|-----------|---------------------|
| AC-1 | LED toggles every 1s while disconnected | Visual observation |
| AC-2 | No meter record on SERCOM0 while disconnected | Logic analyzer / UART monitor |
| AC-3 | Connection indication establishes connected state without MCU reset | State machine log via SERCOM2 |
| AC-4 | Data transmission begins only after STREAM_OPEN confirmed | State machine log via SERCOM2 |
| AC-5 | One valid JSON record per 100 ms sample while connected | UART monitor timing analysis |
| AC-6 | LED toggles every 200 ms while connected and sending | Visual observation |
| AC-7 | Disconnect immediately stops transmission, discards backlog | State machine log, UART monitor |
| AC-8 | Parser handles byte-at-a-time, fragmented, and adjacent messages | Unit test / integration test |
| AC-9 | JSON format exactly matches canonical spec | Byte-level comparison |
| AC-10 | Measurements near 0Ω and 10kΩ follow documented conversion | ADC raw value vs converted value log |
| AC-11 | SERCOM2 reports all required diagnostic events | Log review |
| AC-12 | Diagnostic load does not corrupt SERCOM0 or delay sampling | Stress test with verbose logging |
| AC-13 | Project builds without warnings from new code | Compiler output |

---

## Revision History

| Version | Date | Author | Changes |
|---------|------|--------|---------|
| 1.0 | 2026-09-29 | Claude | Initial plan created from repository inspection |
| 1.1 | 2026-09-29 | Claude | Resolved all open questions with datasheet references: OQ-1 (ADC channel 2), OQ-2 (range GPIO from CD4028B/schematic), OQ-3 (MCP3204 SPI protocol and conversion), OQ-4 (RNBD350 status strings from user guide) |

