# Test Plan: hal_spi (M-08)

**Module:** hal_spi  
**ID:** M-08  
**Layer:** HAL  
**Date:** 2026-09-30  
**Status:** Draft

---

## 1. Module Under Test

The `hal_spi` module provides SPI abstraction for MCP3204 ADC communication.

**Public Interface:**
```c
void hal_spi_init(void);
void hal_spi_transfer(const uint8_t* tx, uint8_t* rx, size_t len);
```

---

## 2. Test Coverage Matrix

### 2.1 hal_spi_init

| Test ID | Scenario | Expected Result | Category |
|---------|----------|-----------------|----------|
| TCI-075 | Call init | Configures SERCOM5 SPI peripheral | Expected |

### 2.2 hal_spi_transfer

| Test ID | Scenario | Expected Result | Category |
|---------|----------|-----------------|----------|
| TCI-076 | Transfer multiple bytes | All TX bytes sent | Expected |
| TCI-077 | Transfer multiple bytes | All RX bytes received | Expected |
| TCI-078 | Zero length transfer | No operation, no CS toggle | Boundary |
| TCI-079 | NULL rx buffer | TX only (ignores RX) | Expected |
| TCI-080 | CS assertion | CS asserted before first byte | Expected |
| TCI-081 | CS deassertion | CS deasserted after last byte | Expected |

---

## 3. SPI Configuration

| Parameter | Value |
|-----------|-------|
| Peripheral | SERCOM5 |
| Clock | 1 MHz |
| Mode | 0 (CPOL=0, CPHA=0) |
| Data Order | MSB first |

---

## 4. Dependencies

| Dependency | Mock Strategy |
|------------|---------------|
| MCC SPI driver | Mock on host; real on target |
| hal_gpio (M-07) | Mock: verify CS pin writes |

---

## 5. Traceability

| Test ID | Enabler Story | Acceptance Criteria |
|---------|---------------|---------------------|
| TCI-075-081 | E1-002 | SPI communication with ADC |

---

## 6. Test Declarations

See: [test-declarations.cpp](test-declarations.cpp)
