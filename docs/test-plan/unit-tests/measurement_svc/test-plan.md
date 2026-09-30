# Test Plan: measurement_svc (M-03)

**Module:** measurement_svc  
**ID:** M-03  
**Layer:** Service  
**Date:** 2026-09-30  
**Status:** Draft

---

## 1. Module Under Test

The `measurement_svc` module handles resistance measurement acquisition from the MCP3204 ADC and conversion from raw counts to ohms.

**Public Interface:**
```c
typedef struct {
    uint32_t ohms;
    bool     valid;
} measurement_t;

void          measurement_svc_init(void);
measurement_t measurement_svc_sample(void);
```

---

## 2. Test Coverage Matrix

### 2.1 measurement_svc_init

| Test ID | Scenario | Expected Result | Category |
|---------|----------|-----------------|----------|
| TCI-013 | Call init | Configures range GPIOs via hal_gpio | Expected |

### 2.2 measurement_svc_sample

| Test ID | Scenario | Expected Result | Category |
|---------|----------|-----------------|----------|
| TCI-014 | Normal sample | Reads ADC via hal_spi | Expected |
| TCI-015 | Raw counts received | Converts to ohms correctly | Expected |
| TCI-016 | Valid ADC response | Returns measurement with valid=true | Expected |
| TCI-017 | ADC returns 0 counts | Returns 0 ohms | Boundary |
| TCI-018 | ADC returns 4095 counts (max) | Returns max ohms | Boundary |
| TCI-019 | ADC returns mid-range counts | Returns proportional ohms | Expected |
| TCI-020 | SPI transfer | Sends correct MCP3204 command bytes | Expected |
| TCI-021 | Channel selection | Command byte selects channel 2 | Expected |
| TCI-022 | SPI error | Returns measurement with valid=false | Error |
| TCI-023 | Multiple consecutive samples | Each works correctly | Expected |
| TCI-024 | Timing | Completes within timing budget | Timing |


---

## 3. ADC Conversion Formula

```
MCP3204: 12-bit ADC, Vref = 3.3V
ADC counts range: 0 to 4095

For voltage divider with known reference:
ohms = (counts * REFERENCE_OHMS) / (4095 - counts)

Or linear mapping (depending on circuit):
ohms = (counts * MAX_OHMS) / 4095
```

**Test Values:**
| ADC Counts | Expected Ohms (assuming linear, 10kΩ max) |
|------------|-------------------------------------------|
| 0 | 0 |
| 2047 | ~5000 |
| 4095 | 10000 |

---

## 4. Dependencies

| Dependency | Mock Strategy |
|------------|---------------|
| hal_gpio (M-07) | Mock: verify range pin writes (RES_A, RES_B, RES_C) |
| hal_spi (M-08) | Mock: inject ADC response bytes (CS handled internally by HAL) |

---

## 5. MCP3204 SPI Protocol

```
Command byte format (single-ended, channel 2):
TX: [0x06] [0x00] [0x00]  -- Start bit, single-ended, channel 2
RX: [xx]   [high] [low]   -- Ignore first, high 4 bits, low 8 bits

12-bit result = ((high & 0x0F) << 8) | low
```

---

## 6. Traceability

| Test ID | Enabler Story | Acceptance Criteria |
|---------|---------------|---------------------|
| TCI-013 | E1-004 | Range GPIOs configured at init |
| TCI-014, TCI-020-023 | E1-002 | ADC read via SPI |
| TCI-015, TCI-017-019 | E1-003 | Counts converted to ohms |
| TCI-016, TCI-024 | E1-002 | Valid flag indicates success/failure |
| TCI-025 | E1-002 | Repeated samples work |
| TCI-026 | E5-002 | Sample fits in 100ms budget |

---

## 7. Test Declarations

See: [test-declarations.cpp](test-declarations.cpp)
