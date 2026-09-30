# Test Plan: app_state (M-02)

**Module:** app_state  
**ID:** M-02  
**Layer:** Application  
**Date:** 2026-09-30  
**Status:** Draft

---

## 1. Module Under Test

The `app_state` module manages the centralized application state machine, tracking BLE connection state (disconnected, connected, streaming) and providing query functions for other modules.

**Public Interface:**
```c
typedef enum {
    APP_STATE_DISCONNECTED,
    APP_STATE_CONNECTED,
    APP_STATE_STREAMING
} app_state_t;

void        app_state_init(void);
app_state_t app_state_get(void);
void        app_state_on_connect(void);
void        app_state_on_stream_open(void);
void        app_state_on_disconnect(void);
bool        app_state_is_streaming(void);
```

---

## 2. Test Coverage Matrix

### 2.1 app_state_init

| Test ID | Scenario | Expected Result | Category |
|---------|----------|-----------------|----------|
| TCI-001 | Call init | State is DISCONNECTED | Expected |

### 2.2 app_state_get

| Test ID | Scenario | Expected Result | Category |
|---------|----------|-----------------|----------|
| TCI-002 | After various state changes | Returns current state value | Expected |

### 2.3 app_state_on_connect

| Test ID | Scenario | Expected Result | Category |
|---------|----------|-----------------|----------|
| TCI-003 | When DISCONNECTED | Transitions to CONNECTED | Expected |
| TCI-004 | When CONNECTED | Remains CONNECTED | Boundary |
| TCI-005 | When STREAMING | Remains STREAMING | Boundary |

### 2.4 app_state_on_stream_open

| Test ID | Scenario | Expected Result | Category |
|---------|----------|-----------------|----------|
| TCI-006 | When CONNECTED | Transitions to STREAMING | Expected |
| TCI-007 | When DISCONNECTED | Remains DISCONNECTED (invalid) | Error |
| TCI-008 | When STREAMING | Remains STREAMING | Boundary |

### 2.5 app_state_on_disconnect

| Test ID | Scenario | Expected Result | Category |
|---------|----------|-----------------|----------|
| TCI-009 | When CONNECTED | Transitions to DISCONNECTED | Expected |
| TCI-010 | When STREAMING | Transitions to DISCONNECTED | Expected |

### 2.6 app_state_is_streaming

| Test ID | Scenario | Expected Result | Category |
|---------|----------|-----------------|----------|
| TCI-011 | When STREAMING | Returns true | Expected |
| TCI-012 | When DISCONNECTED or CONNECTED | Returns false | Expected |

---

## 3. State Transition Diagram

```
                    on_connect()
    DISCONNECTED ────────────────► CONNECTED
          ▲                            │
          │                            │ on_stream_open()
          │                            ▼
          └──────────────────────── STREAMING
                 on_disconnect()
```

**Valid Transitions:**
- DISCONNECTED → CONNECTED (on_connect)
- CONNECTED → STREAMING (on_stream_open)
- CONNECTED → DISCONNECTED (on_disconnect)
- STREAMING → DISCONNECTED (on_disconnect)

**Invalid/No-op Transitions:**
- DISCONNECTED → STREAMING (on_stream_open while disconnected)
- CONNECTED → CONNECTED (on_connect while connected)
- STREAMING → STREAMING (on_stream_open while streaming)

---

## 4. Dependencies

| Dependency | Mock Strategy |
|------------|---------------|
| None | Pure logic module, no mocks needed |

---

## 5. Traceability

| Test ID | Enabler Story | Acceptance Criteria |
|---------|---------------|---------------------|
| TCI-001 | E2-006 | System initializes to known state |
| TCI-002 | E2-004 | State queryable by other modules |
| TCI-003-005 | E2-004 | Connect event handled correctly |
| TCI-006-008 | E2-004 | STREAM_OPEN event handled correctly |
| TCI-009-010 | E2-006 | Disconnect resets state |
| TCI-011-012 | E2-004 | Streaming flag accurate |

---

## 6. Test Declarations

See: [test-declarations.cpp](test-declarations.cpp)
