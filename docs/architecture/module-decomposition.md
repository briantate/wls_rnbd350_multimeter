# Module Decomposition

**Project:** Bluetooth Ohmmeter Firmware  
**Version:** 1.0  
**Date:** 2026-09-30  
**Status:** Draft

---

## 1. Overview

This document enumerates all software modules in the Bluetooth Ohmmeter firmware, their responsibilities, interfaces, owned data, and layer placement. Each module encapsulates one design secret (a likely-to-change decision) following Parnas-style information hiding.

---

## 2. Layer Definitions

| Layer       | Responsibility                                    | Dependency Rule                    |
|-------------|---------------------------------------------------|-----------------------------------|
| Application | Super-loop orchestration, state machine           | Calls Service and HAL             |
| Service     | Domain logic, protocol handling, timing policy    | Calls HAL only                    |
| HAL         | Hardware abstraction (portable interfaces)        | Calls Driver only                 |
| Driver      | MCC-generated peripheral access                   | Accesses hardware directly        |
| Platform    | MCU hardware registers                            | N/A                               |

---

## 3. Module Catalog

### 3.1 Application Layer

---

#### M-01: main

| Attribute       | Value                                                      |
|-----------------|------------------------------------------------------------|
| **ID**          | M-01                                                       |
| **Name**        | main                                                       |
| **Layer**       | Application                                                |
| **Purpose**     | Entry point and super-loop orchestration                   |
| **Design secret**| Super-loop scheduling policy and initialization order     |
| **Source files**| `src/app/main.c`                                           |

**Responsibilities:**
- Initialize all modules in correct order
- Run infinite super-loop polling all subsystems
- Coordinate tick-based sample triggering

**Public Interface:**
```c
int main(void);  // Entry point; does not return
```

**Owned Data:**
- None (delegates to app_state)

**Dependencies:**
- M-02 (app_state)
- M-03 (measurement_svc)
- M-04 (ble_svc)
- M-05 (led_svc)
- M-06 (diag_svc)
- M-10 (hal_tick)

**Traceability:**
- E6-001: Bare-metal cooperative main loop

---

#### M-02: app_state

| Attribute       | Value                                                      |
|-----------------|------------------------------------------------------------|
| **ID**          | M-02                                                       |
| **Name**        | app_state                                                  |
| **Layer**       | Application                                                |
| **Purpose**     | Centralized application state machine                      |
| **Design secret**| State transition logic and state encoding                 |
| **Source files**| `src/app/app_state.h`, `src/app/app_state.c`               |

**Responsibilities:**
- Track BLE connection state (disconnected, connected, streaming)
- Provide `streaming_enabled` flag to other modules
- Handle state transitions on BLE events

**Public Interface:**
```c
typedef enum {
    APP_STATE_DISCONNECTED,
    APP_STATE_CONNECTED,      // BLE connected, STREAM_OPEN not yet received
    APP_STATE_STREAMING       // STREAM_OPEN received, data TX enabled
} app_state_t;

void        app_state_init(void);
app_state_t app_state_get(void);
void        app_state_on_connect(void);
void        app_state_on_stream_open(void);
void        app_state_on_disconnect(void);
bool        app_state_is_streaming(void);
```

**Owned Data:**
- `static app_state_t current_state;`

**Dependencies:**
- None (pure logic)

**Traceability:**
- E2-004: STREAM_OPEN detection
- E2-006: Disconnect handling
- E3-002, E3-003: LED rate depends on state

---

### 3.2 Service Layer

---

#### M-03: measurement_svc

| Attribute       | Value                                                      |
|-----------------|------------------------------------------------------------|
| **ID**          | M-03                                                       |
| **Name**        | measurement_svc                                            |
| **Layer**       | Service                                                    |
| **Purpose**     | Resistance measurement acquisition and conversion          |
| **Design secret**| ADC-to-ohms conversion formula; range configuration       |
| **Source files**| `src/services/measurement_svc.h`, `src/services/measurement_svc.c` |

**Responsibilities:**
- Initialize range selection GPIOs (RES_A, RES_B, RES_C)
- Read raw ADC value from MCP3204 Channel 2 via SPI
- Convert 12-bit ADC counts to resistance in ohms
- Provide latest measurement to caller

**Public Interface:**
```c
typedef struct {
    uint32_t ohms;          // Resistance in ohms
    bool     valid;         // True if measurement is valid
} measurement_t;

void          measurement_svc_init(void);
measurement_t measurement_svc_sample(void);
```

**Owned Data:**
- Range GPIO configuration (compile-time constant)

**Dependencies:**
- M-07 (hal_gpio) - for range selection and chip select
- M-08 (hal_spi) - for ADC communication

**Traceability:**
- E1-002: Read ADC
- E1-003: Convert to ohms
- E1-004: Configure range

---

#### M-04: ble_svc

| Attribute       | Value                                                      |
|-----------------|------------------------------------------------------------|
| **ID**          | M-04                                                       |
| **Name**        | ble_svc                                                    |
| **Layer**       | Service                                                    |
| **Purpose**     | RNBD350 communication and JSON data transmission           |
| **Design secret**| RNBD350 status message parsing; JSON format               |
| **Source files**| `src/services/ble_svc.h`, `src/services/ble_svc.c`         |

**Responsibilities:**
- Initialize RNBD350 UART interface
- Parse incoming status messages (%CONNECT%, %DISCONNECT%, %STREAM_OPEN%)
- Handle byte-at-a-time, fragmented, and adjacent messages
- Format resistance as JSON: `{"Meter":{"ohm":<value>}}\r\n`
- Transmit JSON via UART when streaming enabled
- Discard pending data on disconnect

**Public Interface:**
```c
typedef enum {
    BLE_EVENT_NONE,
    BLE_EVENT_CONNECT,
    BLE_EVENT_DISCONNECT,
    BLE_EVENT_STREAM_OPEN
} ble_event_t;

void        ble_svc_init(void);
ble_event_t ble_svc_poll(void);                        // Check for events
void        ble_svc_transmit_measurement(uint32_t ohms);
void        ble_svc_discard_pending(void);
```

**Owned Data:**
- RX parse buffer (static, ~32 bytes)
- TX format buffer (static, ~48 bytes)
- Parser state machine state

**Dependencies:**
- M-09 (hal_uart) - for SERCOM0 communication

**Traceability:**
- E2-002: UART init
- E2-003: Status message parsing
- E2-004: STREAM_OPEN detection
- E2-005: JSON formatting and TX
- E2-006: Disconnect handling

---

#### M-05: led_svc

| Attribute       | Value                                                      |
|-----------------|------------------------------------------------------------|
| **ID**          | M-05                                                       |
| **Name**        | led_svc                                                    |
| **Layer**       | Service                                                    |
| **Purpose**     | Status LED control with state-dependent blink rate         |
| **Design secret**| LED timing policy (1 Hz disconnected, 5 Hz connected)     |
| **Source files**| `src/services/led_svc.h`, `src/services/led_svc.c`         |

**Responsibilities:**
- Track elapsed time since last toggle
- Toggle LED at 1000 ms when disconnected
- Toggle LED at 200 ms when streaming
- Query app_state for current connection state

**Public Interface:**
```c
void led_svc_init(void);
void led_svc_update(uint32_t elapsed_ms);  // Called each tick
```

**Owned Data:**
- `static uint32_t ms_since_toggle;`
- `static bool led_state;`

**Dependencies:**
- M-02 (app_state) - for streaming state
- M-07 (hal_gpio) - for LED pin control
- M-10 (hal_tick) - for timing (indirect via elapsed_ms parameter)

**Traceability:**
- E3-002: 1 Hz blink when disconnected
- E3-003: 5 Hz blink when connected/streaming

---

#### M-06: diag_svc

| Attribute       | Value                                                      |
|-----------------|------------------------------------------------------------|
| **ID**          | M-06                                                       |
| **Name**        | diag_svc                                                   |
| **Layer**       | Service                                                    |
| **Purpose**     | Diagnostic logging to dedicated UART                       |
| **Design secret**| Diagnostic message format; verbosity filtering            |
| **Source files**| `src/services/diag_svc.h`, `src/services/diag_svc.c`       |

**Responsibilities:**
- Output timestamped diagnostic messages to SERCOM2
- Filter messages by compile-time verbosity level
- Format: `[<timestamp_ms>] [<LEVEL>] [<SUBSYSTEM>] <message>`
- Never affect SERCOM0 traffic or sample timing

**Public Interface:**
```c
typedef enum {
    DIAG_DEBUG,
    DIAG_INFO,
    DIAG_WARN,
    DIAG_ERROR
} diag_level_t;

// Compile-time verbosity (set in build config)
#ifndef DIAG_VERBOSITY
#define DIAG_VERBOSITY DIAG_INFO
#endif

void diag_svc_init(void);

// Macro to compile out low-priority messages
#define DIAG_LOG(level, subsys, msg, ...) \
    do { if ((level) >= DIAG_VERBOSITY) diag_svc_log((level), (subsys), (msg), ##__VA_ARGS__); } while(0)

void diag_svc_log(diag_level_t level, const char* subsys, const char* fmt, ...);
```

**Owned Data:**
- TX format buffer (static, ~128 bytes)

**Dependencies:**
- M-09 (hal_uart) - for SERCOM2 communication
- M-10 (hal_tick) - for timestamp

**Traceability:**
- E4-002: Timestamped diagnostic output
- E4-003: Compile-time verbosity control

---

### 3.3 HAL (Hardware Abstraction Layer)

---

#### M-07: hal_gpio

| Attribute       | Value                                                      |
|-----------------|------------------------------------------------------------|
| **ID**          | M-07                                                       |
| **Name**        | hal_gpio                                                   |
| **Layer**       | HAL                                                        |
| **Purpose**     | GPIO abstraction for LED, range selection, chip select     |
| **Design secret**| Pin assignments; MCC GPIO driver API                      |
| **Source files**| `src/hal/hal_gpio.h`, `src/hal/hal_gpio.c` (target), `test/mocks/hal_gpio_mock.c` (host) |

**Responsibilities:**
- Abstract GPIO read/write operations
- Define logical pin names independent of physical mapping
- Provide mock implementation for host-side testing

**Public Interface:**
```c
typedef enum {
    HAL_PIN_LED,
    HAL_PIN_RES_A,
    HAL_PIN_RES_B,
    HAL_PIN_RES_C,
    HAL_PIN_SPI_CS
} hal_pin_t;

void hal_gpio_init(void);
void hal_gpio_write(hal_pin_t pin, bool state);
bool hal_gpio_read(hal_pin_t pin);
void hal_gpio_toggle(hal_pin_t pin);
```

**Owned Data:**
- Pin mapping table (compile-time constant)

**Dependencies:**
- MCC GPIO driver (target only)

**Traceability:**
- E1-004: Range GPIO configuration
- E3-002, E3-003: LED control

---

#### M-08: hal_spi

| Attribute       | Value                                                      |
|-----------------|------------------------------------------------------------|
| **ID**          | M-08                                                       |
| **Name**        | hal_spi                                                    |
| **Layer**       | HAL                                                        |
| **Purpose**     | SPI abstraction for MCP3204 ADC communication              |
| **Design secret**| MCC SPI driver API; SPI mode and clock configuration      |
| **Source files**| `src/hal/hal_spi.h`, `src/hal/hal_spi.c` (target), `test/mocks/hal_spi_mock.c` (host) |

**Responsibilities:**
- Initialize SPI peripheral (SERCOM5, 1 MHz, Mode 0)
- Provide synchronous transfer function
- Abstract chip select assertion (or delegate to hal_gpio)

**Public Interface:**
```c
void    hal_spi_init(void);
void hal_spi_transfer(const uint8_t* tx, uint8_t* rx, size_t len);
```

**Owned Data:**
- None (stateless after init)

**Dependencies:**
- MCC SPI driver (target only)

**Traceability:**
- E1-002: SPI communication with MCP3204

---

#### M-09: hal_uart

| Attribute       | Value                                                      |
|-----------------|------------------------------------------------------------|
| **ID**          | M-09                                                       |
| **Name**        | hal_uart                                                   |
| **Layer**       | HAL                                                        |
| **Purpose**     | UART abstraction for BLE module and diagnostic output      |
| **Design secret**| MCC UART driver API; SERCOM assignments                   |
| **Source files**| `src/hal/hal_uart.h`, `src/hal/hal_uart.c` (target), `test/mocks/hal_uart_mock.c` (host) |

**Responsibilities:**
- Initialize UART peripherals (SERCOM0 for BLE, SERCOM2 for diag)
- Provide non-blocking TX and RX functions
- Abstract two UART channels via channel ID

**Public Interface:**
```c
typedef enum {
    HAL_UART_BLE,   // SERCOM0 - RNBD350
    HAL_UART_DIAG   // SERCOM2 - Diagnostics
} hal_uart_channel_t;

void   hal_uart_init(hal_uart_channel_t ch);
bool   hal_uart_tx_ready(hal_uart_channel_t ch);
void   hal_uart_tx_byte(hal_uart_channel_t ch, uint8_t byte);
void   hal_uart_tx_string(hal_uart_channel_t ch, const char* str);
bool   hal_uart_rx_available(hal_uart_channel_t ch);
uint8_t hal_uart_rx_byte(hal_uart_channel_t ch);
```

**Owned Data:**
- Per-channel RX ring buffer (if buffered RX needed)

**Dependencies:**
- MCC UART driver (target only)

**Traceability:**
- E2-002: BLE UART init
- E4-002: Diagnostic UART output

---

#### M-10: hal_tick

| Attribute       | Value                                                      |
|-----------------|------------------------------------------------------------|
| **ID**          | M-10                                                       |
| **Name**        | hal_tick                                                   |
| **Layer**       | HAL                                                        |
| **Purpose**     | System tick abstraction (1 ms timebase)                    |
| **Design secret**| Timer peripheral configuration; tick ISR mechanism        |
| **Source files**| `src/hal/hal_tick.h`, `src/hal/hal_tick.c` (target), `test/mocks/hal_tick_mock.c` (host) |

**Responsibilities:**
- Initialize TC0 for 1 ms periodic interrupt
- Maintain 32-bit millisecond counter
- Provide flag for main loop to detect tick events

**Public Interface:**
```c
void     hal_tick_init(void);
uint32_t hal_tick_get_ms(void);           // Milliseconds since boot
bool     hal_tick_check_flag(void);       // Returns true once per ms, clears flag
void     hal_tick_clear_flag(void);       // Manual flag clear if needed

// For test mocking
void     hal_tick_advance_ms(uint32_t ms); // Mock only: simulate time passing
```

**Owned Data:**
- `static volatile uint32_t tick_ms;`
- `static volatile bool tick_flag;`

**Dependencies:**
- MCC Timer driver (target only)

**Traceability:**
- E5-002: 100 ms sample interval (derived from 1 ms tick)

---

## 4. Module Interaction Summary

```
                    +-------+
                    | main  |
                    | M-01  |
                    +---+---+
                        |
        +-------+-------+-------+-------+
        |       |       |       |       |
        v       v       v       v       v
   +--------+ +-----+ +-----+ +------+ +--------+
   |app_state| |meas | |ble  | |led   | |diag    |
   |  M-02   | |_svc | |_svc | |_svc  | |_svc    |
   |         | |M-03 | |M-04 | |M-05  | |M-06    |
   +---------+ +--+--+ +--+--+ +--+---+ +---+----+
                  |       |       |         |
                  v       v       v         v
             +--------+--------+--------+--------+
             |hal_gpio|hal_spi |hal_uart|hal_tick|
             | M-07   | M-08   | M-09   | M-10   |
             +--------+--------+--------+--------+
                  |       |       |         |
                  v       v       v         v
             +----------------------------------------+
             |         MCC Drivers (unmodified)       |
             +----------------------------------------+
                              |
                              v
             +----------------------------------------+
             |      PIC32CM3204GV00064 Hardware       |
             +----------------------------------------+
```

---

## 5. Data Ownership Matrix

| Data Item                  | Owner Module | Accessors                    |
|----------------------------|--------------|------------------------------|
| Application state          | M-02         | M-01, M-05                   |
| Current measurement        | M-03         | M-01 (passes to M-04)        |
| BLE parser state           | M-04         | M-04 only                    |
| BLE TX buffer              | M-04         | M-04 only                    |
| LED toggle state           | M-05         | M-05 only                    |
| Diagnostic TX buffer       | M-06         | M-06 only                    |
| System tick counter        | M-10         | All (read-only via API)      |
| GPIO pin states            | Hardware     | M-07 (abstracted)            |

---

## 6. Static Memory Budget

| Module   | RAM Usage (bytes) | Notes                              |
|----------|-------------------|------------------------------------|
| M-02     | 4                 | State enum + padding               |
| M-03     | 8                 | Measurement struct                 |
| M-04     | 80                | RX buffer (32) + TX buffer (48)    |
| M-05     | 8                 | Timer + state                      |
| M-06     | 128               | Format buffer                      |
| M-09     | 112               | RX ring buffers (64 BLE + 32 DIAG) + metadata |
| M-10     | 8                 | Tick counter + flag                |
| Stack    | ~512              | Worst-case call depth              |
| **Total**| **~860**          | Well under 4 KB SRAM budget        |
