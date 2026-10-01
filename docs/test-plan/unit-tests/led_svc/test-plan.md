# Test Plan: led_svc (M-05)

**Module:** led_svc  
**ID:** M-05  
**Layer:** Service  
**Date:** 2026-09-30  
**Status:** Draft

---

## 1. Module Under Test

The `led_svc` module controls the status LED with state-dependent blink rates: 1 Hz when disconnected, 5 Hz when streaming.

**Public Interface:**
```c
void led_svc_init(void);
void led_svc_update(uint32_t elapsed_ms);
```

---

## 2. Test Coverage Matrix

### 2.1 led_svc_init

| Test ID | Scenario | Expected Result | Category |
|---------|----------|-----------------|----------|
| TCI-047 | Call init | LED turned off initially | Expected |

### 2.2 led_svc_update

| Test ID | Scenario | Expected Result | Category |
|---------|----------|-----------------|----------|
| TCI-048 | Disconnected state | Toggles every 1000 ms | Expected |
| TCI-049 | Streaming state | Toggles every 200 ms | Expected |
| TCI-050 | Before 1000 ms (disconnected) | No toggle | Boundary |
| TCI-051 | At 1000 ms (disconnected) | Toggles | Boundary |
| TCI-052 | Before 200 ms (streaming) | No toggle | Boundary |
| TCI-053 | At 200 ms (streaming) | Toggles | Boundary |
| TCI-054 | State changes mid-cycle | Adjusts to new rate | Expected |
| TCI-055 | Accumulates elapsed time | Time adds up across calls | Expected |
| TCI-056 | Resets after toggle | Timer resets to 0 | Expected |

---

## 3. Timing Behavior

| State | Toggle Period | Blink Rate |
|-------|---------------|------------|
| DISCONNECTED | 1000 ms | 0.5 Hz (1 Hz full cycle) |
| CONNECTED | 200 ms | 2.5 Hz (5 Hz full cycle) |
| STREAMING | 200 ms | 2.5 Hz (5 Hz full cycle) |

---

## 4. Dependencies

| Dependency | Mock Strategy |
|------------|---------------|
| app_state (M-02) | Mock: return desired state |
| hal_gpio (M-07) | Mock: verify LED toggle calls |

---

## 5. Traceability

| Test ID | Enabler Story | Acceptance Criteria |
|---------|---------------|---------------------|
| TCI-047 | E3-002 | LED starts in known state |
| TCI-048, TCI-050-051, TCI-055-056 | E3-002 | 1 Hz blink when disconnected |
| TCI-049, TCI-052-054 | E3-003 | 5 Hz blink when streaming |

---

## 6. Test Declarations

See: [test-declarations.cpp](test-declarations.cpp)
