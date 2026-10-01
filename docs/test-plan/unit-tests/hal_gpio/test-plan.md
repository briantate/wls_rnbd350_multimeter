# Test Plan: hal_gpio (M-07)

**Module:** hal_gpio  
**ID:** M-07  
**Layer:** HAL  
**Date:** 2026-09-30  
**Status:** Draft

---

## 1. Module Under Test

The `hal_gpio` module provides GPIO abstraction for LED, range selection pins, and SPI chip select.

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

---

## 2. Test Coverage Matrix

### 2.1 hal_gpio_init

| Test ID | Scenario | Expected Result | Category |
|---------|----------|-----------------|----------|
| TCI-065 | Call init | Configures all pins | Expected |

### 2.2 hal_gpio_write

| Test ID | Scenario | Expected Result | Category |
|---------|----------|-----------------|----------|
| TCI-066 | Write to LED pin | LED state set | Expected |
| TCI-067 | Write to RES_A pin | RES_A state set | Expected |
| TCI-068 | Write to RES_B pin | RES_B state set | Expected |
| TCI-069 | Write to RES_C pin | RES_C state set | Expected |
| TCI-070 | Write to SPI_CS pin | SPI_CS state set | Expected |
| TCI-073 | Write to invalid pin | No effect, no crash | Error |

### 2.3 hal_gpio_read

| Test ID | Scenario | Expected Result | Category |
|---------|----------|-----------------|----------|
| TCI-071 | Read pin state | Returns current state | Expected |
| TCI-074 | Read invalid pin | Returns false | Error |

### 2.4 hal_gpio_toggle

| Test ID | Scenario | Expected Result | Category |
|---------|----------|-----------------|----------|
| TCI-072 | Toggle pin | State inverted | Expected |

---

## 3. Pin Mapping

| Logical Pin | Physical Pin | Direction | Purpose |
|-------------|--------------|-----------|---------|
| HAL_PIN_LED | PA15 | Output | Status LED |
| HAL_PIN_RES_A | PA16 | Output | Range select bit A |
| HAL_PIN_RES_B | PA17 | Output | Range select bit B |
| HAL_PIN_RES_C | PA18 | Output | Range select bit C |
| HAL_PIN_SPI_CS | PA19 | Output | MCP3204 chip select |

---

## 4. Dependencies

| Dependency | Mock Strategy |
|------------|---------------|
| MCC GPIO driver | Mock on host; real on target |

---

## 5. Traceability

| Test ID | Enabler Story | Acceptance Criteria |
|---------|---------------|---------------------|
| TCI-065 | E1-004, E3-002 | GPIO pins configured |
| TCI-066, TCI-072 | E3-002 | LED control works |
| TCI-067-069, TCI-071 | E1-004 | Range pins controllable |
| TCI-070 | E1-002 | SPI CS controllable |
| TCI-073-074 | - | Robustness against invalid input |

---

## 6. Test Declarations

See: [test-declarations.cpp](test-declarations.cpp)
