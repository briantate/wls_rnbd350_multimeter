---
name: tdd-write-passing-implementation
description: Implement the "GREEN" path in Test Driven Development. Write the minimum code to make a specified failing test pass without breaking existing tests. Use when doing TDD to implement the "Green" path, following Red-Green-Refactor rhythm. Triggers on phrases like "make test pass", "green phase", "TDD green", "implement for test", "write implementation for", or when the user wants the simplest code to satisfy a failing test.
---

# TDD: Write Passing Implementation (GREEN Phase)

This skill implements the GREEN phase of Test Driven Development. It writes the **minimum** code necessary to make a specified failing test pass while ensuring all other existing tests continue to pass.

## Core Principles

You are implementing the GREEN phase of the Red-Green-Refactor TDD cycle:
- Red: Write a failing test
- **GREEN**: Write minimal code to pass (THIS SKILL)
- Refactor: Clean up while tests stay green

The implementation MUST be as simple as possible. If hardcoding a return value makes all tests pass, **do that**. Complexity is added only when tests demand it.

## Golden Rule

**Write the simplest code that makes the test pass.**

- If one hardcoded value satisfies all tests, return that value.
- If a simple `if` statement is enough, use that—not a lookup table, state machine, or abstraction.
- Never anticipate future tests. Only satisfy the tests that exist now.
- "Simple" means fewer lines, fewer branches, less cognitive load.

## Anti-Patterns (FORBIDDEN)

1. **Over-engineering** — Do not write more code than the test requires
2. **Anticipating future tests** — Only satisfy the current test, not hypothetical ones
3. **Refactoring during green** — Save cleanup for the Refactor phase
4. **Modifying tests** — Never change the test; only change the implementation
5. **Breaking existing tests** — All previously passing tests must still pass
6. **Adding abstractions** — No helper functions, no generalization beyond what tests require
7. **Modifying multiple functions** — If the test requires changes to more than one function, STOP. The test likely depends on an unimplemented helper. Go back to RED phase and test/implement the helper function first, then return to this test.

## Input Contract

The user provides:
1. **Test file path** — The file containing the failing test (e.g., `firmware/test/temp_ctrl/test_temp_ctrl.cpp`)
2. **Test identifier** — Either:
   - Test name (e.g., `test_temp_ctrl_init_rejects_null_state`)
   - Full TEST macro (e.g., `TEST(TempCtrl, test_temp_ctrl_init_rejects_null_state)`)
3. **Source file path** — The `.c` file to modify (e.g., `firmware/src/temp_ctrl/temp_ctrl.c`)
4. **Header file path** — The `.h` file (e.g., `firmware/src/temp_ctrl/temp_ctrl.h`)

If any input is ambiguous or missing, ASK the user before proceeding.

## Output Contract

The skill produces:
- Modified source file (`.c`) with the minimum implementation to pass the test
- No changes to the header file unless absolutely required for compilation
- No changes to the test file
- All existing tests continue to pass

## Workflow

### Step 1: Read and Understand the Failing Test

Read the test file and locate the specified test. Extract:
- What function is being called
- What inputs are provided
- What output/behavior is expected (the assertion)

### Step 2: Read All Existing Tests

Read all tests in the test file to understand:
- What behaviors are already tested and passing
- What constraints the existing implementation must satisfy
- What values/behaviors must NOT be broken

### Step 3: Read the Current Implementation

Read the source file (`.c`) and header file (`.h`) to understand:
- Current function signatures
- Current implementation (if any)
- Existing return values and behavior

### Step 4: Plan the Simplest Change

Determine the **absolute minimum** code change to make the new test pass. Consider in order of preference:

1. **Hardcode a return value** — If returning a constant satisfies all tests
2. **Add a single `if` statement** — If one condition check is needed
3. **Add minimal logic** — Only if simpler approaches break existing tests

Example thought process:
```
New test expects: func(NULL) returns ERROR_NULL
Existing tests expect: func(valid_ptr) returns SUCCESS

Simplest solution:
  if (ptr == NULL) { return ERROR_NULL; }
  return SUCCESS;
```

### Step 5: Apply Coding Standards

Before writing, invoke the `/coding-standards` skill mentally or reference it to ensure:
- Correct naming conventions (`snake_case` functions, `_t` suffix for types)
- Required includes (`<stdbool.h>`, `<stdint.h>`, `<stddef.h>`)
- Proper indentation (4 spaces)
- No HAL boundary violations

### Step 6: Write the Implementation

Modify **only** the source file with the minimum change. Do not:
- Add comments explaining the change
- Refactor surrounding code
- Add defensive checks not required by tests
- Generalize the solution

### Step 7: Run Tests to Verify GREEN State

After writing the implementation, **run the tests** to confirm:
1. The new test now passes
2. All existing tests still pass

#### Test Execution Strategy

**Detect the test framework and runner:**

1. Look for `project.yml` in the `firmware/` directory → Use **Ceedling**: `ceedling test:all`
2. Look for `Makefile` with test targets → Use `make test`
3. Look for `CMakeLists.txt` with test configuration → Use `ctest` or `cmake --build . --target test`

**Try to run tests locally first:**

```bash
cd firmware && ceedling test:all
```

**If the test command fails with "command not found" or similar:**

The test framework may not be installed on the host system. ASK the user:

> "The test framework (ceedling/make/cmake) doesn't appear to be installed locally. Is there a Docker container running with the build tools? If so, what is the container name or ID so I can run the tests there?"

**Running tests in Docker:**

If the user provides a container name/ID, run tests via:
```bash
docker exec -w /workdir <container_name> ceedling test:all
```

Adjust the working directory (`-w`) based on where the project is mounted in the container.

#### Interpreting Test Results

- **All tests pass** → GREEN phase complete, proceed to completion message
- **New test fails** → Implementation is incorrect; revise with a simpler or corrected approach
- **Existing test fails** → Implementation broke something; adjust to satisfy ALL tests
- **Compilation error** → Fix syntax/include issues before re-running

If tests fail, iterate on the implementation until all tests pass. Do NOT declare GREEN phase complete until tests actually pass.

## Examples

### Example 1: Hardcoded Return Value

**Test:**
```cpp
TEST(TempCtrl, test_temp_ctrl_get_version_returns_one)
{
    LONGS_EQUAL(1, temp_ctrl_get_version());
}
```

**Simplest implementation:**
```c
int temp_ctrl_get_version(void) {
    return 1;
}
```

Do NOT write a version constant, do NOT read from a config — just return `1`.

### Example 2: Single Condition Check

**Failing test:**
```cpp
TEST(TempCtrl, test_temp_ctrl_init_rejects_null_state)
{
    temp_ctrl_status_t status = temp_ctrl_init(NULL, &config);
    LONGS_EQUAL(TEMP_CTRL_ERR_NULL_PTR, status);
}
```

**Existing passing test:**
```cpp
TEST(TempCtrl, test_temp_ctrl_init_succeeds_with_valid_args)
{
    temp_ctrl_status_t status = temp_ctrl_init(&state, &config);
    LONGS_EQUAL(TEMP_CTRL_OK, status);
}
```

**Simplest implementation:**
```c
temp_ctrl_status_t temp_ctrl_init(temp_ctrl_state_t *state,
                                   const temp_ctrl_config_t *config) {
    if (state == NULL) {
        return TEMP_CTRL_ERR_NULL_PTR;
    }
    return TEMP_CTRL_OK;
}
```

### Example 3: Resist Over-Engineering

**Failing test:**
```cpp
TEST(Calculator, test_add_two_and_three_returns_five)
{
    LONGS_EQUAL(5, calculator_add(2, 3));
}
```

**WRONG (over-engineered):**
```c
int calculator_add(int a, int b) {
    return a + b;  // Too general! No test requires this
}
```

**CORRECT (simplest):**
```c
int calculator_add(int a, int b) {
    return 5;  // The ONLY thing the test requires
}
```

The general solution comes later when more tests demand it.

## Checklist Before Completing

- [ ] Read the failing test and understood the required behavior
- [ ] Read all existing tests to know what must not break
- [ ] Read current source and header files
- [ ] Wrote the **simplest** code that makes the test pass
- [ ] Did NOT over-engineer or anticipate future tests
- [ ] Did NOT modify test files
- [ ] Did NOT refactor existing code
- [ ] Modified only ONE function (if multiple needed, STOP and check dependencies)
- [ ] Code follows project coding standards
- [ ] Ran tests and verified ALL tests pass (new and existing)
- [ ] If local tools unavailable, asked user about Docker container

## What to Say When Done

```
## GREEN Phase Complete ✓

### Change Made:
[Brief description of the minimal change]

### File Modified:
- `path/to/source.c`

### Test Results:
- Tests run: X
- Tests passed: X
- Tests failed: 0

All tests pass. The implementation satisfies the new test without breaking existing tests.

### Next Steps:
You may now proceed to the REFACTOR phase to clean up the code while keeping tests green.
```
