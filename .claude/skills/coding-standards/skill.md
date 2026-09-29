---
name: coding-standards
description: Apply the project's embedded C coding standards. Use when generating, reviewing, or refactoring C code, header files, or test files in this project. Covers file organization, naming conventions, indentation, comments, error handling, HAL boundary rules, and test conventions.
---

# Embedded C Coding Standards

This standard adapts the Google C++ Style Guide for embedded C. It is
opinionated, deliberate, and prioritizes readability, testability, and
deterministic behavior over cleverness. Apply it whenever you generate,
review, or refactor C code in this project.

The standard assumes:

- C99 or later (C11 preferred where available)
- Static analysis enabled (clang-tidy, cppcheck, or equivalent)
- A separate HAL layer that owns all hardware access
- Unit testing with CppUTest or an equivalent host-runnable framework

## Required Standard Includes

Every `.c` file MUST explicitly include the standard headers for types it uses.
Do not assume these are transitively included via other headers.

| Type Used | Required Include |
|-----------|------------------|
| `bool`, `true`, `false` | `<stdbool.h>` |
| `uint8_t`, `int16_t`, `uint32_t`, `int32_t`, etc. | `<stdint.h>` |
| `NULL`, `size_t`, `ptrdiff_t` | `<stddef.h>` |
| `memcpy`, `memset`, `strlen`, `strncpy` | `<string.h>` |

Example of correct includes in a source file:

```c
#include "module/module.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "hal/hal_i2c.h"
```

**NEVER** rely on headers including these for you. If you use `bool`,
you include `<stdbool.h>`. If you use `uint32_t`, you include `<stdint.h>`.
This prevents build failures when headers change or when compiling on
different platforms.

## File Organization

### File names

- Lowercase `snake_case`. No camelCase. No spaces. No hyphens.
- Header files end in `.h`; source files end in `.c`; test files end in
  `.cpp` (CppUTest harness) or `.c` depending on framework.
- One module per file pair. The pair shares a base name:
  `temp_controller.h` and `temp_controller.c`. The test for that pair
  is `test_temp_controller.cpp`.

```
inc/temp_controller.h
src/temp_controller.c
test/test_temp_controller.cpp
```

### Header guards

Use `#ifndef` / `#define` / `#endif` guards. The macro form is
`PROJECT_MODULE_FILE_H_`.

```c
#ifndef TEMPCTL_TEMP_CONTROLLER_H_
#define TEMPCTL_TEMP_CONTROLLER_H_

/* declarations */

#endif  /* TEMPCTL_TEMP_CONTROLLER_H_ */
```

Do not use `#pragma once`. The explicit guard is portable, greppable,
and obvious.

### Header organization

A header file follows this exact section order:

1. Header guard open
2. Standard system includes (`<stdint.h>`, `<stdbool.h>`, ...)
3. Project includes (`"hal/hal_i2c.h"`, ...)
4. Forward declarations (if any)
5. Public type definitions (typedefs, structs, enums)
6. Public function declarations
7. Header guard close

### Source organization

A `.c` file follows this exact section order:

1. Header for the module being implemented (`#include "temp_controller.h"`)
2. System includes used only by the implementation
3. Project includes used only by the implementation
4. File-scoped constants (`static const ...`)
5. File-scoped statics (variables and helper functions, prefixed `s_`)
6. Public function definitions, in the order they appear in the header
7. Private (`static`) function definitions

## Naming Conventions

### Functions

`snake_case` with a module prefix. The prefix is the module name.

```c
void temp_controller_init(temp_controller_t *ctl, const temp_controller_config_t *cfg);
temp_controller_status_t temp_controller_update(temp_controller_t *ctl, int16_t reading_c, uint32_t now_ms);
bool bme280_read_temperature(int16_t *out_centi_c);
```

The prefix prevents collisions in flat C namespaces. Every public
function has a prefix; private (`static`) functions in a `.c` file may
omit the prefix if the meaning stays clear.

### Variables

- **Locals:** `snake_case`. Short and meaningful.
- **File-scoped statics:** `snake_case` prefixed with `s_`
  (`s_last_reading_ms`).
- **Globals:** Avoid. If unavoidable, prefix with `g_`
  (`g_uart_buffer`). Document why a global was unavoidable in a comment.
- **Function parameters:** `snake_case`. Use `out_` prefix for
  caller-provided output buffers (`int16_t *out_temperature_c`).

### Types

Typedef'd structs and enums use `snake_case` with a `_t` suffix.

```c
typedef struct {
    int16_t setpoint_c;
    int16_t hysteresis_band_c;
    uint32_t sample_period_ms;
} temp_controller_config_t;

typedef enum {
    TEMP_STATUS_OK = 0,
    TEMP_STATUS_INVALID_CONFIG,
    TEMP_STATUS_SENSOR_FAULT,
    TEMP_STATUS_OVER_TEMPERATURE,
} temp_controller_status_t;
```

Do not use `typedef` to hide pointer types. `temp_controller_t *` is
clearer than a typedef'd `temp_controller_handle`.

### Constants

- **Macros and enum values:** `UPPER_SNAKE_CASE`.
  ```c
  #define BME280_I2C_ADDRESS 0x76
  #define TEMP_CONTROLLER_MAX_BUTTONS 8
  ```
- **`const` variables:** `k` prefix followed by CamelCase
  (`kMaxRetries`, `kDefaultTimeoutMs`). This matches Google's C++
  convention and distinguishes runtime constants from preprocessor
  symbols.
  ```c
  static const uint32_t kBmeReadTimeoutMs = 50;
  static const int16_t kMinSetpointC = -40;
  ```

### No Magic Numbers

**Never use literal numeric values directly in code or tests.** All
meaningful numbers must be named constants (macros, enums, or `const`
variables).

**Forbidden:**
```c
if (temperature_c < -4000) { ... }        // What is -4000?
config.sample_period_ms = 1000;           // Why 1000?
for (int i = 0; i < 8; i++) { ... }       // Why 8?
```

**Required:**
```c
#define TEMP_CTRL_MIN_VALID_CENTI_C  (-4000)
#define TEMP_CTRL_DEFAULT_SAMPLE_MS  (1000U)
#define MAX_SENSOR_COUNT             (8)

if (temperature_c < TEMP_CTRL_MIN_VALID_CENTI_C) { ... }
config.sample_period_ms = TEMP_CTRL_DEFAULT_SAMPLE_MS;
for (int i = 0; i < MAX_SENSOR_COUNT; i++) { ... }
```

**Exceptions (magic numbers allowed):**
- `0`, `1`, `-1` when semantically obvious (loop start, increment, error sentinel)
- Array index arithmetic where the meaning is clear from context
- Bit shifts for flag definitions (`1 << 3`)

**In tests:** Use the same named constants from the module header. If the
test needs a boundary value (e.g., `min - 1`), express it as an
expression using the constant:
```c
config.setpoint_centi_c = TEMP_CTRL_MIN_VALID_CENTI_C - 1;  // Just below minimum
```

This ensures tests stay synchronized with implementation when limits change.

## Indentation, Braces, and Line Length

### Indentation

- **Four spaces.** No tabs.
- One indent level per nesting level. No half-indents, no aligning to a
  brace.

### Braces

Opening brace on the same line for functions and control flow. Closing
brace on its own line. Always use braces, even for single-statement
bodies.

```c
if (status != TEMP_STATUS_OK) {
    return status;
}

for (size_t i = 0; i < count; ++i) {
    process(items[i]);
}

void temp_controller_init(temp_controller_t *ctl,
                          const temp_controller_config_t *cfg) {
    /* body */
}
```

### Line length

100 columns max. Wrap long argument lists by aligning continuations
under the opening parenthesis (or by indenting one level if alignment
would push too far right).

## Comments

### Block vs inline

- Use `/* ... */` for block comments, including file headers and
  multi-line explanations.
- Use `//` for inline comments at the end of a line of code.
- Be consistent within a file.

### Doxygen for public API

Every public function declaration in a header file gets a Doxygen-style
comment. Use the `@brief`, `@param`, `@return` form.

```c
/**
 * @brief Update the controller given a new temperature reading.
 *
 * @param ctl       Caller-owned controller object initialized with
 *                  temp_controller_init().
 * @param reading_c Current ambient temperature in centi-degrees C.
 * @param now_ms    Monotonic millisecond timestamp.
 * @return Status code; see temp_controller_status_t.
 */
temp_controller_status_t temp_controller_update(
    temp_controller_t *ctl,
    int16_t reading_c,
    uint32_t now_ms);
```

### What to comment

- Explain *why*, not *what*. The code already says what.
- Comment hidden invariants, surprising constraints, and references to
  external specs (datasheets, RFCs).
- Do not write comments that restate the function name.

```c
/* Bad */
i++;  /* increment i */

/* Good */
i++;  /* skip the BME280 chip-id byte; addresses 0x88..0x9F are calib */
```

## Error Handling

### Return codes

All public functions that can fail return a status enum from the
project-wide `*_status_t` set. The caller must check the return value.

```c
temp_controller_status_t temp_controller_update(...);

/* Caller */
status = temp_controller_update(&ctl, reading, now_ms);
if (status != TEMP_STATUS_OK) {
    handle_error(status);
}
```

### Forbidden

- **No exceptions.** This is C; exceptions do not exist. Do not
  emulate them with `setjmp`/`longjmp`. They break stack discipline and
  defeat static analysis.
- **No silent failures.** Every fail path returns a status code. No
  `return;` with no value or no documented success guarantee.
- **No assert as production logic.** `assert()` is for invariants that
  cannot be triggered by valid inputs; never use it to validate
  caller-supplied data.

### Output parameters

When a function produces a value but can also fail, the value goes
through an output parameter and the return is a status code.

```c
temp_status_t bme280_read_temperature(int16_t *out_centi_c);
```

## HAL Boundary Rules

The project enforces a strict HAL boundary. Business logic (the
temperature controller, application code, state machines) **never**
directly touches hardware. Every hardware interaction goes through the
HAL abstraction layer.

### Forbidden in business logic

- Direct register access (`*(volatile uint32_t *)0x40004400 = ...`)
- ST HAL calls (`HAL_I2C_Master_Transmit`, `HAL_GPIO_WritePin`, ...)
- CMSIS calls (`SysTick_Config`, `__NOP()`, ...)
- Direct interrupt-controller manipulation (`NVIC_EnableIRQ`)

### Allowed in business logic

- HAL abstraction calls (`hal_i2c_read`, `hal_gpio_set`, `hal_time_now_ms`)
- Standard C library functions that are pure (`memcpy`, `strncmp`,
  fixed-width integer types, `bool`)

### Where the boundary lives

The HAL abstraction lives in `inc/hal/` and `src/hal/`. The HAL
implementation for a target lives in `src/hal/<target>/`
(`src/hal/stm32l475/`, `src/hal/host/`, etc.). The business-logic
modules `#include "hal/hal_i2c.h"` and never `#include "stm32l4xx_hal.h"`.

### Reviewing for HAL boundary

When reviewing code, search for any direct hardware reference in a
file outside `src/hal/<target>/`. Any match is a violation.

## Test Conventions

### File layout

Test files mirror the source structure. For every `src/temp_controller.c`
there is a `test/test_temp_controller.cpp`.

```
src/temp_controller.c          → test/test_temp_controller.cpp
src/hal/host/hal_i2c_host.c   → test/hal/host/test_hal_i2c_host.cpp
```

### Test names

Test functions follow the form `test_<unit>_<behavior>`. Each name
states the unit under test and the specific behavior being verified.

```c
TEST(TempController, test_temp_controller_init_validates_setpoint);
TEST(TempController, test_temp_controller_update_asserts_heater_below_band);
TEST(TempController, test_temp_controller_update_emits_overtemp_event);
```

### Test independence

- Each test sets up its own state. Do not rely on test ordering.
- Use the framework's setup/teardown (`setUp`/`tearDown` in CppUTest)
  for any state shared across tests in a group.
- Mocks and fakes for HAL boundaries; do not call real hardware from
  tests.

### Coverage expectation

Branch coverage of 100% on the business-logic modules. The HAL
implementation does not need 100%; integration tests on hardware cover
those paths.

## Summary Checklist

When generating or reviewing C code in this project, verify:

- [ ] All required standard includes present (`<stdbool.h>`, `<stdint.h>`, `<stddef.h>`)
- [ ] File names are `snake_case`, header guards are `PROJECT_MODULE_FILE_H_`
- [ ] Functions, locals, and statics use `snake_case`; types end in `_t`
- [ ] Constants use `UPPER_SNAKE` (macros/enums) or `k` prefix (const variables)
- [ ] **No magic numbers** — all meaningful literals are named constants
- [ ] Four-space indent, braces on same line, max 100 columns
- [ ] Doxygen comments on every public function declaration
- [ ] Public failure paths return a `*_status_t` enum
- [ ] No direct register, HAL, or CMSIS access outside `src/hal/<target>/`
- [ ] Test file structure mirrors source; test names follow
      `test_<unit>_<behavior>`
- [ ] Tests reference named constants from headers, not hardcoded values
- [ ] No `#pragma once`, no exceptions, no `setjmp`, no global state
      without `g_` prefix and a justifying comment
