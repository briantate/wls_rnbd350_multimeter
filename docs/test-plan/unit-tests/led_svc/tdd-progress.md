# TDD Progress: led_svc

**Module:** led_svc (M-05)
**Test Plan:** [test-plan.md](test-plan.md)
**Test Declarations:** [test-declarations.cpp](test-declarations.cpp)
**Source File:** `firmware/src/services/led_svc.c`
**Test File:** `firmware/tests/led_svc/test_led_svc.cpp`

**Started:** 2026-10-01
**Last Updated:** 2026-10-01
**Status:** Complete

## Decisions (from coordinator, 2026-10-01)

1. Pre-existing implementation in `led_svc.c` was reset to empty stubs so every test can go RED first.
2. Real `app_state` is used (no mock); state is driven via `app_state_init()`, `app_state_on_connect()`, `app_state_on_stream_open()`. Declarations' `app_state_*` mock expectations are replaced accordingly.
3. CONNECTED state blinks at 1000 ms (same as DISCONNECTED); only STREAMING uses 200 ms. **Note:** this deviates from test-plan.md section 3, which lists CONNECTED at 200 ms; the test plan should be updated.
4. `setup()` wraps `led_svc_init()` in `mock().disable()` / `mock().enable()` to avoid unexpected `hal_gpio_write` calls.

## Summary

| Category | Total | Implemented | Remaining |
|----------|-------|-------------|-----------|
| Init - Expected | 1 | 1 | 0 |
| Update - Expected | 5 | 5 | 0 |
| Update - Boundary | 4 | 4 | 0 |
| **TOTAL** | **10** | **10** | **0** |

## Test Checklist

### led_svc_init - Expected Behavior
- [x] TCI-047 `Init_TurnsLedOff` - 2026-10-01

### led_svc_update - Expected Behavior
- [x] TCI-048 `Update_WhenDisconnected_TogglesAt1000ms` - 2026-10-01
- [x] TCI-049 `Update_WhenStreaming_TogglesAt200ms` - 2026-10-01
- [x] TCI-054 `Update_StateChangesMidCycle_AdjustsRate` - 2026-10-01 (passed on first run; see notes)
- [x] TCI-055 `Update_AccumulatesElapsedTime` - 2026-10-01 (passed on first run; see notes)
- [x] TCI-056 `Update_ResetsAfterToggle` - 2026-10-01

### led_svc_update - Boundary
- [x] TCI-050 `Update_Before1000ms_NoToggle` - 2026-10-01
- [x] TCI-051 `Update_At1000ms_Toggles` - 2026-10-01
- [x] TCI-052 `Update_Before200ms_NoToggle` - 2026-10-01 (passed on first run; see notes)
- [x] TCI-053 `Update_At200ms_Toggles` - 2026-10-01 (passed on first run; see notes)

## Implementation Notes

### TCI-047: Init_TurnsLedOff
- **Date:** 2026-10-01
- **RED:** `hal_gpio_write` expected 1 call, called 0 times (1 failure, 47 tests)
- **Function Modified:** `led_svc_init()`
- **Change:** Calls `hal_gpio_write(HAL_PIN_LED, false)`
- **GREEN:** OK (47 tests, 47 ran, 786 checks)

### TCI-048: Update_WhenDisconnected_TogglesAt1000ms
- **Date:** 2026-10-01
- **RED:** `hal_gpio_toggle` expected 1 call, called 0 times (1 failure, 48 tests)
- **Function Modified:** `led_svc_update()`
- **Change:** Unconditionally calls `hal_gpio_toggle(HAL_PIN_LED)` (minimal; threshold deferred to TCI-050)
- **GREEN:** OK (48 tests, 48 ran, 787 checks)

### TCI-050: Update_Before1000ms_NoToggle
- **Date:** 2026-10-01
- **RED:** Unexpected call to `hal_gpio_toggle` (1 failure, 49 tests)
- **Function Modified:** `led_svc_update()`
- **Change:** Toggle only when `elapsed_ms >= LED_SVC_BLINK_PERIOD_DISCONNECTED_MS`
- **GREEN:** OK (49 tests, 49 ran, 788 checks)

### TCI-051: Update_At1000ms_Toggles
- **Date:** 2026-10-01
- **RED:** `hal_gpio_toggle` expected 1 call, called 0 times (1 failure, 50 tests)
- **Functions Modified:** `led_svc_update()` (primary), `led_svc_init()` (accumulator reset)
- **Change:** Added static `s_ms_since_toggle` accumulator; update adds `elapsed_ms` and compares against 1000 ms
- **Deviation:** Two functions touched. The `led_svc_init()` reset of `s_ms_since_toggle` is required for test isolation (static state otherwise carries over from TCI-048's 1000 ms and breaks TCI-050/051).
- **GREEN:** OK (50 tests, 50 ran, 789 checks)

### TCI-056: Update_ResetsAfterToggle
- **Date:** 2026-10-01
- **RED:** Unexpected call to `hal_gpio_toggle` on second update (1 failure, 51 tests)
- **Function Modified:** `led_svc_update()`
- **Change:** Reset `s_ms_since_toggle = 0` after toggle
- **GREEN:** OK (51 tests, 51 ran, 791 checks)

### TCI-049: Update_WhenStreaming_TogglesAt200ms
- **Date:** 2026-10-01
- **RED:** `hal_gpio_toggle` expected 1 call, called 0 times (1 failure, 52 tests)
- **Function Modified:** `led_svc_update()`
- **Change:** Threshold selected via `app_state_is_streaming()` (200 ms streaming, 1000 ms otherwise)
- **GREEN:** OK (52 tests, 52 ran, 792 checks)

### TCI-052: Update_Before200ms_NoToggle
- **Date:** 2026-10-01
- **RED:** NOT observed - passed on first run, OK (53 tests, 53 ran, 793 checks)
- **Cause:** Boundary already satisfied by the threshold comparison added in TCI-049
- **Validation:** Mutation check - temporarily set streaming threshold to 199 ms; test failed (1 failure, 53 tests). Source restored; suite OK (53 tests)
- **Function Modified:** None

### TCI-053: Update_At200ms_Toggles
- **Date:** 2026-10-01
- **RED:** NOT observed - passed on first run, OK (54 tests, 54 ran, 794 checks)
- **Cause:** Accumulation (TCI-051) + streaming threshold (TCI-049) already implement this
- **Validation:** Mutation check - replaced `+=` accumulation with `=`; TCI-053 and TCI-051 failed. Source restored; suite OK (54 tests)
- **Function Modified:** None

### TCI-054: Update_StateChangesMidCycle_AdjustsRate
- **Date:** 2026-10-01
- **RED:** NOT observed - passed on first run, OK (55 tests, 55 ran, 795 checks)
- **Cause:** TCI-049 implementation reads `app_state_is_streaming()` on every update call, so a mid-cycle change already applies immediately
- **Validation:** Mutation check - latched streaming state on first call; TCI-054 failed (plus TCI-049/053). Source restored; suite OK (55 tests)
- **Function Modified:** None

### TCI-055: Update_AccumulatesElapsedTime
- **Date:** 2026-10-01
- **RED:** NOT observed - passed on first run, OK (56 tests, 56 ran, 797 checks)
- **Cause:** Accumulator added in TCI-051 already satisfies multi-call accumulation
- **Validation:** Mutation check - replaced `+=` with `=`; TCI-055 failed (plus TCI-051/053/054). Source restored; suite OK (56 tests)
- **Function Modified:** None

## Open Items

- `test-plan.md` section 3 lists CONNECTED at 200 ms; the coordinator decided 1000 ms. The plan should be updated, and a test for CONNECTED -> 1000 ms (not in the current plan) is recommended.
- `test-declarations.cpp` still uses `app_state_*` mock expectations; it should be aligned with the real-app_state approach.
