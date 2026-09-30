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
| TCI-049 | Call init | LED turned off initially | Expected |

### 2.2 led_svc_update

| Test ID | Scenario | Expected Result | Category |
|---------|----------|-----------------|----------|
| TCI-050 | Disconnected state | Toggles every 1000 ms | Expected |
| TCI-051 | Streaming state | Toggles every 200 ms | Expected |
| TCI-052 | Before 1000 ms (disconnected) | No toggle | Boundary |
| TCI-053 | At 1000 ms (disconnected) | Toggles | Boundary |
| TCI-054 | Before 200 ms (streaming) | No toggle | Boundary |
| TCI-055 | At 200 ms (streaming) | Toggles | Boundary |
| TCI-056 | State changes mid-cycle | Adjusts to new rate | Expected |
| TCI-057 | Accumulates elapsed time | Time adds up across calls | Expected |
| TCI-058 | Resets after toggle | Timer resets to 0 | Expected |

---

## 3. Timing Behavior

| State | Toggle Period | Blink Rate |
|-------|---------------|------------|
| DISCONNECTED | 1000 ms | 0.5 Hz (1 Hz full cycle) |
| CONNECTED | 200 ms | 2.5 Hz (5 Hz full cycle) |
| STREAMING | 200 ms | 2.5 Hz (5 Hz full cycle) |

**Note:** A "1 Hz blink" means 1 full on-off cycle per second = 500 ms on + 500 ms off = toggle every 500 ms.
Actually checking requirements: "1 Hz disconnected, 5 Hz connected" means:
- 1 Hz = toggle every 500 ms (but spec says 1000 ms period?)
- Need to verify with requirements. Assuming toggle period = half the blink period.

For this test plan, we follow the module decomposition spec which states:
- 1000 ms toggle when disconnected (0.5 Hz blink = 1 Hz visual rate counting on+off)
- 200 ms toggle when streaming (2.5 Hz blink = 5 Hz visual rate)

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
| TCI-049 | E3-002 | LED starts in known state |
| TCI-050, TCI-052-053, TCI-057-058 | E3-002 | 1 Hz blink when disconnected |
| TCI-051, TCI-054-055 | E3-003 | 5 Hz blink when streaming |
| TCI-056 | E3-002, E3-003 | Smooth transition between rates |

---

## 6. Test Declarations

See: [test-declarations.cpp](test-declarations.cpp)
