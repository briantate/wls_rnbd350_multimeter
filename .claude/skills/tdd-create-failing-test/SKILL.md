---
name: tdd-create-failing-test
description: Implement the "RED" path in Test Driven Development. Create a single firmware unit test that matches a description in a test plan file. Use this when doing test driven development to implement the "Red" path, following Red-Green-Refactor TDD rhythm. Triggers on phrases like "create failing test", "red phase", "TDD red", "write the test for", "implement test TCI-007", or when the user references a test ID from a test plan.
---

# TDD: Create Failing Test (RED Phase)

This skill implements the RED phase of Test Driven Development. It creates exactly ONE failing test based on a test description from a test plan file.

## Core Principles

You are implementing the RED phase of the Red-Green-Refactor TDD cycle:
- **RED**: Write a failing test (THIS SKILL)
- Green: Write minimal code to pass
- Refactor: Clean up while tests stay green

The test MUST fail when first run. A passing test on first run means either the test is wrong or the implementation already exists (which violates TDD flow).

## Anti-Patterns (FORBIDDEN)

1. **Creating more than one test** — Stop after writing exactly one test
2. **Writing implementation code** — Never touch the code under test
3. **Writing a test that passes immediately** — The test must fail initially
4. **Guessing test details** — Always derive from the test plan specification
5. **Writing a test that depends on unimplemented helper functions** — If your test uses another function to verify the result (e.g., calling `get_X()` to verify `set_X()` worked), that helper function must already have passing tests. Otherwise, test the helper first. Each test should only require changes to ONE function in the GREEN phase.

## Input Contract

The user provides:
1. **Test plan file path** — Points to a markdown file containing test descriptions
   - Located at: `docs/test-plan/unit-tests/<module>/test-plan.md`
   - Example: `docs/test-plan/unit-tests/temp_ctrl/test-plan.md`
2. **Target test file path** — Where the test will be written (e.g., `firmware/tests/temp_ctrl/test_temp_ctrl.cpp`)
3. **Test identifier** — Either:
   - Test ID (e.g., `TCI-007`, `TCU-001`)
   - Test name (e.g., `Init_NullState_ReturnsNullPtrError`)

If any input is ambiguous or missing, ASK the user before proceeding.

## Output Contract

The skill produces exactly ONE new test in CppUTest format following the 4-phase test pattern.

## Workflow

### Step 1: Parse the Test Plan

Read the test plan file and locate the specified test by ID or name. Extract:
- Test ID
- Test Name
- Scenario description
- Expected Result

If the test cannot be found, list available tests from the same section and ask the user to clarify.

### Step 2: Determine TEST_GROUP Membership

Check if the target test file exists:

**If file does not exist:**
- Create the file with full CppUTest boilerplate
- Infer the TEST_GROUP name from the module (e.g., `temp_ctrl` → `TempCtrl`)
- Include appropriate headers based on the module under test

**If file exists:**
- Scan for existing `TEST_GROUP` definitions
- If exactly one group exists, use it
- If multiple groups exist, present the list and ask the user which group this test belongs to, or whether to create a new group

### Step 3: Write the Test Using 4-Phase Pattern

Follow Gerard Meszaros's xUnit Test Patterns (as paraphrased by James Grenning in "TDD for Embedded C"):

```cpp
TEST(GroupName, test_name_from_plan)
{
    // 1. SETUP: Establish preconditions
    //    - Initialize state structures
    //    - Configure test fixtures
    //    - Set up mocks/fakes if needed

    // 2. EXERCISE: Do something to the system
    //    - Call the function under test
    //    - Capture the result

    // 3. VERIFY: Check the expected outcome
    //    - Use CHECK, CHECK_EQUAL, LONGS_EQUAL, etc.
    //    - Assert the expected result from the test plan

    // 4. CLEANUP: Return to initial state
    //    - Usually handled by teardown()
    //    - Only add explicit cleanup if setup allocated resources
}
```

### Step 4: Confirm Completion

After writing the test:
1. Show the user the complete test code
2. Remind them this test MUST fail when first run
3. Do NOT proceed to write implementation code — that's the user's next step (GREEN phase)

## CppUTest Conventions

### File Structure (new file)

```cpp
/**
 * @file test_<module>.cpp
 * @brief Unit tests for <module> module.
 */

#include "CppUTest/TestHarness.h"

extern "C" {
#include "<module>/<module>.h"
// Additional headers as needed
}

TEST_GROUP(ModuleName)
{
    // Shared state for tests in this group
    
    void setup()
    {
        // Common setup for all tests
    }

    void teardown()
    {
        // Common cleanup for all tests
    }
};

TEST(ModuleName, test_name_here)
{
    // 4-phase test body
}
```

### Adding to Existing File

Insert the new TEST() block after the last existing test in the target group, maintaining consistent formatting.

### Assertion Macros

| Assertion | Use Case |
|-----------|----------|
| `CHECK(condition)` | Boolean conditions |
| `CHECK_FALSE(condition)` | Expect false |
| `CHECK_EQUAL(expected, actual)` | Equality with type deduction |
| `LONGS_EQUAL(expected, actual)` | Integer comparison |
| `POINTERS_EQUAL(expected, actual)` | Pointer comparison |
| `STRCMP_EQUAL(expected, actual)` | String comparison |
| `FAIL("message")` | Force failure (placeholder) |

### Test Naming

Follow project conventions: `test_<unit>_<behavior>`

Examples:
- `test_temp_ctrl_init_rejects_null_state`
- `test_bme280_read_returns_err_on_bus_fault`

## Directory Structure

Tests go in `firmware/test/<module>/test_<module>.cpp`:

```
firmware/
├── src/
│   └── temp_ctrl/
│       └── temp_ctrl.c
└── test/
    └── temp_ctrl/
        └── test_temp_ctrl.cpp
```

Create the directory structure if it doesn't exist.

## Example

**User input:**
- Test plan: `docs/test-plan/unit-tests/temp_ctrl/test-plan.md`
- Target file: `firmware/tests/temp_ctrl/test_temp_ctrl.cpp`
- Test ID: `TCI-007`

**From test plan:**
| ID | Test Name | Scenario | Expected Result |
|----|-----------|----------|-----------------|
| TCI-007 | `Init_NullState_ReturnsNullPtrError` | state == NULL | Returns TEMP_CTRL_ERR_NULL_PTR |

**Generated test:**

```cpp
TEST(TempCtrl, test_temp_ctrl_init_rejects_null_state)
{
    // SETUP
    temp_ctrl_config_t config;
    config.setpoint_centi_c = 2500;
    config.hysteresis_centi_c = 100;
    config.over_temp_centi_c = 8500;
    config.min_valid_centi_c = -4000;
    config.max_valid_centi_c = 8500;
    config.sample_period_ms = 1000U;

    // EXERCISE
    temp_ctrl_status_t status = temp_ctrl_init(NULL, &config);

    // VERIFY
    LONGS_EQUAL(TEMP_CTRL_ERR_NULL_PTR, status);

    // CLEANUP
    // None needed — no dynamic allocation
}
```

## Checklist Before Completing

- [ ] Exactly ONE test created (not zero, not multiple)
- [ ] Test derives from the specified test plan entry
- [ ] 4-phase pattern followed (Setup, Exercise, Verify, Cleanup)
- [ ] No implementation code written
- [ ] Test file placed in `firmware/test/<module>/`
- [ ] TEST_GROUP membership resolved (existing or new, with user confirmation if ambiguous)
- [ ] User reminded that the test must FAIL on first run
- [ ] Test only requires changes to ONE function in GREEN phase
- [ ] Any helper functions used for verification are already tested and working
