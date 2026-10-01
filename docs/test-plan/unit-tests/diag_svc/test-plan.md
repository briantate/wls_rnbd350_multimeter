# Test Plan: diag_svc (M-06)

**Module:** diag_svc  
**ID:** M-06  
**Layer:** Service  
**Date:** 2026-09-30  
**Status:** Draft

---

## 1. Module Under Test

The `diag_svc` module provides timestamped diagnostic logging to a dedicated UART channel with compile-time verbosity filtering.

**Public Interface:**
```c
typedef enum {
    DIAG_DEBUG,
    DIAG_INFO,
    DIAG_WARN,
    DIAG_ERROR
} diag_level_t;

void diag_svc_init(void);
void diag_svc_log(diag_level_t level, const char* subsys, const char* fmt, ...);

#define DIAG_LOG(level, subsys, msg, ...) \
    do { if ((level) >= DIAG_VERBOSITY) diag_svc_log(...); } while(0)
```

---

## 2. Test Coverage Matrix

### 2.1 diag_svc_init

| Test ID | Scenario | Expected Result | Category |
|---------|----------|-----------------|----------|
| TCI-057 | Call init | Configures DIAG UART channel | Expected |

### 2.2 diag_svc_log

| Test ID | Scenario | Expected Result | Category |
|---------|----------|-----------------|----------|
| TCI-058 | Normal log | Output includes timestamp | Expected |
| TCI-059 | Normal log | Output includes level string | Expected |
| TCI-060 | Normal log | Output includes subsystem | Expected |
| TCI-061 | Normal log | Output includes message | Expected |
| TCI-062 | Level below VERBOSITY | No output | Expected |
| TCI-063 | Level at VERBOSITY | Output produced | Boundary |
| TCI-064 | Level above VERBOSITY | Output produced | Expected |

---

## 3. Output Format

```
[<timestamp_ms>] [<LEVEL>] [<SUBSYSTEM>] <message>\r\n
```

Example:
```
[00012345] [INFO] [BLE] Connection established\r\n
```

---

## 4. Verbosity Levels

| Level | Value | Use Case |
|-------|-------|----------|
| DIAG_DEBUG | 0 | Detailed trace information |
| DIAG_INFO | 1 | General operational info |
| DIAG_WARN | 2 | Warning conditions |
| DIAG_ERROR | 3 | Error conditions |

Default `DIAG_VERBOSITY` is `DIAG_INFO`.

---

## 5. Dependencies

| Dependency | Mock Strategy |
|------------|---------------|
| hal_uart (M-09) | Mock: capture output strings |
| hal_tick (M-10) | Mock: return controlled timestamp |

---

## 6. Traceability

| Test ID | Enabler Story | Acceptance Criteria |
|---------|---------------|---------------------|
| TCI-057 | E4-002 | Diagnostic UART initialized |
| TCI-058-061 | E4-002 | Formatted timestamped output |
| TCI-062-064 | E4-003 | Verbosity filtering works |

---

## 7. Test Declarations

See: [test-declarations.cpp](test-declarations.cpp)
