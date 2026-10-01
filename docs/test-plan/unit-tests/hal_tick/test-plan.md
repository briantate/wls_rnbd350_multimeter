# Test Plan: hal_tick (M-10)

**Module:** hal_tick  
**ID:** M-10  
**Layer:** HAL  
**Date:** 2026-09-30  
**Status:** Draft

---

## 1. Module Under Test

The `hal_tick` module provides a 1 ms system tick timebase for timing operations.

**Public Interface:**
```c
void     hal_tick_init(void);
uint32_t hal_tick_get_ms(void);
```

---

## 2. Test Coverage Matrix

### 2.1 hal_tick_init

| Test ID | Scenario | Expected Result | Category |
|---------|----------|-----------------|----------|
| TCI-096 | Call init | Starts TC0 timer | Expected |

### 2.2 hal_tick_get_ms

| Test ID | Scenario | Expected Result | Category |
|---------|----------|-----------------|----------|
| TCI-097 | After init | Returns milliseconds since boot | Expected |
| TCI-098 | Over time | Value increments | Expected |
| TCI-099 | Wraparound safe | Arithmetic works across 32-bit boundary | Boundary |

---

## 3. Timer Configuration

| Parameter | Value |
|-----------|-------|
| Peripheral | TC0 |
| Period | 1 ms |
| Interrupt | Enabled |
| Counter | 32-bit rollover |

---

## 4. Dependencies

| Dependency | Mock Strategy |
|------------|---------------|
| MCC Timer driver | Mock on host; real on target |

---

## 5. Traceability

| Test ID | Enabler Story | Acceptance Criteria |
|---------|---------------|---------------------|
| TCI-096-099 | E5-002 | 100 ms sample interval derived from 1 ms tick |

---

## 6. Test Declarations

See: [test-declarations.cpp](test-declarations.cpp)
