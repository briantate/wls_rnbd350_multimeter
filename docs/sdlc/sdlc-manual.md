# Embedded Firmware Software Development Lifecycle Manual

This document describes the complete software development lifecycle (SDLC) for embedded firmware projects using AI-assisted development with human review gates. It provides step-by-step guidance for taking a project from initial concept through validated, production-ready firmware.

---

## Table of Contents

1. [Overview](#1-overview)
2. [Prerequisites](#2-prerequisites)
3. [Phase 1: Requirements Gathering](#3-phase-1-requirements-gathering)
4. [Phase 2: Architecture Design](#4-phase-2-architecture-design)
5. [Phase 3: Test Planning](#5-phase-3-test-planning)
6. [Phase 4: Scaffolding](#6-phase-4-scaffolding)
7. [Phase 5: Test-Driven Implementation](#7-phase-5-test-driven-implementation)
8. [Phase 6: Integration & System Testing](#8-phase-6-integration--system-testing)
9. [Phase 7: Release](#9-phase-7-release)
10. [Traceability](#10-traceability)
11. [CI/CD Integration](#11-cicd-integration)
12. [Adapting for Safety-Critical Projects](#12-adapting-for-safety-critical-projects)
13. [Appendix A: Templates](#appendix-a-templates)
14. [Appendix B: Tool Reference](#appendix-b-tool-reference)

---

## 1. Overview

### 1.1 Purpose

This SDLC provides a structured, repeatable process for developing embedded firmware with:
- **AI-assisted automation** for architecture, test planning, scaffolding, and TDD implementation
- **Human review gates** at critical decision points to ensure quality and alignment
- **Full traceability** between requirements, tests, and implementation
- **Continuous integration** with automated quality checks

### 1.2 Process Model

This SDLC follows a **V-Model** structure adapted for iterative, test-driven development:

```
Requirements Brief ──────────────────────────────────────── System Validation
        │                                                        ▲
        ▼                                                        │
  User Stories ──────────────────────────────────────── Integration Testing
        │                                                        ▲
        ▼                                                        │
  Architecture ──────────────────────────────────────────── HIL Testing
        │                                                        ▲
        ▼                                                        │
   Test Plans ◄─────────── Traceability ──────────────► Unit Tests
        │                                                        ▲
        ▼                                                        │
  Scaffolding ───────────────► TDD Implementation ───────────────┘
```

Each left-side phase has a corresponding right-side verification activity, with traceability links connecting them.

### 1.3 Human Review Gates

The following gates require human review and approval before proceeding:

| Gate | After Phase | What to Review |
|------|-------------|----------------|
| **G1** | Requirements Brief | Problem statement, constraints, success criteria |
| **G2** | User Stories | Completeness, acceptance criteria, priorities |
| **G3** | Architecture | Module decomposition, HAL boundaries, ADRs |
| **G4** | Test Plans | Coverage completeness, test declarations |
| **G5** | Scaffolding | Compiles cleanly, interfaces match architecture |
| **G6** | Each Module TDD | Code review, coverage report, module functionality |
| **G7** | Integration | All modules work together, system behavior correct |
| **G8** | Release | Release criteria met, documentation complete |

### 1.4 Review Records

Each gate review must be documented to provide evidence that reviews occurred. Review records capture:
- **Who** reviewed (name/role)
- **When** the review occurred (date)
- **What** was reviewed (artifact list)
- **Outcome** (approved, approved with comments, rejected)
- **Findings** (issues identified and resolutions)

**Storage:**
- Design reviews (G1-G5, G7-G8): `docs/reviews/<gate>-<date>.md`
- Code reviews (G6): Pull Request history in Git (PR comments and approvals serve as review record)

**Minimum for compliance:** For code reviews, PR approval history is sufficient. For design reviews, create a review record using the template in [Appendix A.4](#a4-review-record-template).

---

## 2. Prerequisites

### 2.1 Required Starting Artifact

Before entering this SDLC, you **must have** a **Product Requirements Brief**. This document provides the foundation for all downstream work.

If you have informal customer input (emails, conversations, rough ideas), work with the customer to formalize it into a Product Requirements Brief before proceeding.

**Required sections:**

1. **Problem Statement** — What problem does this firmware solve? Who is the user?
2. **Target Hardware** — MCU family, development board, custom hardware specs
3. **Key Features** — Bulleted list of must-have functionality
4. **Constraints** — Power budget, memory limits, timing requirements, cost targets
5. **Success Criteria** — Measurable outcomes that define "done"
6. **Milestones and Timeline** — Customer commitments, key dates, external dependencies

See [Appendix A: Templates](#appendix-a-templates) for a complete template.

### 2.2 Configuration Management

Before development begins, establish configuration management:

- [ ] Git repository initialized
- [ ] Configuration Management Plan documented (`docs/sdlc/cm-plan.md`)
- [ ] Branching strategy defined
- [ ] Commit message format agreed

See [Appendix A.3](#a3-configuration-management-plan-template) for CM plan template.

### 2.3 Development Environment

Ensure the following are configured:

- [ ] Git repository initialized
- [ ] Docker container with build tools:
  - Target compiler (GCC ARM, Clang, vendor toolchain)
  - Unit test framework (CppUTest, Unity, Google Test)
  - Code coverage reporting (gcovr, lcov)
  - Static analysis (Clang-Tidy + Cppcheck recommended; alternatives: PC-lint, Coverity)
- [ ] CI/CD pipeline configured (GitHub Actions, GitLab CI, Jenkins, Azure DevOps)
- [ ] Claude Code with project skills and agents available

> **Note:** Static analysis tools must be in the Docker container to ensure consistent results between local development and CI/CD. The same container used locally should be the same image used in CI.

### 2.3 Project Directory Structure

```
project-root/
├── .claude/
│   ├── agents/           # AI agent definitions
│   └── skills/           # AI skill definitions
├── docs/
│   ├── architecture/     # Architecture artifacts
│   ├── requirements/     # Requirements and user stories
│   └── test-plan/        # Test plans and declarations
│       ├── unit-tests/   # Per-module unit test artifacts
│       ├── simulation/   # Simulation test plans (Renode, QEMU, etc.)
│       ├── hil/          # Hardware-in-the-loop test plans
│       └── manual/       # Manual test procedures (UI, physical inspection)
└── firmware/
    ├── src/              # Production source code
    ├── tests/            # Unit tests (CppUTest)
    └── scaffolding/      # Generated scaffolding (temporary)
```

---

## 3. Phase 1: Requirements Gathering

### 3.1 Objective

Transform the Product Requirements Brief into structured, traceable user stories with clear acceptance criteria.

### 3.2 Process

#### Step 1: Review Product Requirements Brief

Read the Product Requirements Brief and verify it contains all required sections. If any section is incomplete:
- Work with stakeholders to fill gaps
- Do NOT proceed until the brief is complete

#### Step 2: Generate User Stories

Using the requirements brief, create user stories in the format:

```
As a [role], I want [capability] so that [benefit].

Acceptance Criteria:
- [ ] Criterion 1
- [ ] Criterion 2
```

**Tips:**
- Each key feature should map to one or more user stories
- Acceptance criteria should be testable
- Include non-functional requirements (power, timing, memory) as constraints on stories

#### Step 3: Value and Size Estimation (Human Activity)

This step requires two facilitated meetings using relative estimation (bubble sort method):

**Meeting 1: Stakeholder Value Assessment**
- Attendees: Product owner, customer representative, stakeholders
- Activity: Compare stories pairwise to determine relative business value
- Output: Stories ranked by value (1 = highest value)

**Meeting 2: Development Team Sizing**
- Attendees: Development team
- Activity: Compare stories pairwise to determine relative implementation effort
- Output: Stories ranked by size (1 = smallest effort)

#### Step 4: Create Project Backlog

Using the value and size rankings, create the project backlog:

1. **Calculate ROI** — Value rank ÷ Size rank (higher = better ROI)
2. **Order backlog** — Sort by ROI, adjusted for dependencies and risk
3. **Create burn-up chart template** — Track planned vs. actual completion over time

**Tools:** Jira, Azure DevOps, Linear, Trello, or spreadsheet (see template in Appendix A)

> **Note:** This step is human-driven. AI may assist with backlog management in the future, but for now this is a human gate with human-generated artifacts.

### 3.3 Output Artifacts

| Artifact | Location | Description |
|----------|----------|-------------|
| Product Requirements Brief | `docs/requirements/product-brief.md` | Starting input document |
| User Stories | `docs/requirements/user-stories.md` | Structured user stories |
| Project Backlog | Jira / Azure DevOps / Spreadsheet | Prioritized backlog with ROI (human-generated) |
| Burn-up Chart | Project tracking tool | Progress tracking template (human-generated) |

### 3.4 Gate G2: User Stories Review

**Before proceeding, verify:**

- [ ] Every key feature from the brief has at least one user story
- [ ] Each user story has testable acceptance criteria
- [ ] Value assessment meeting completed (stakeholder ranking)
- [ ] Size estimation meeting completed (team ranking)
- [ ] Project backlog created with ROI calculations
- [ ] Burn-up chart template prepared
- [ ] Stakeholders have reviewed and approved the prioritized backlog
- [ ] Constraints (power, memory, timing) are captured

**Reviewer:** Product owner, technical lead, or customer representative

---

## 4. Phase 2: Architecture Design

### 4.1 Objective

Decompose user stories into a modular firmware architecture with clear boundaries, dependencies, and architectural decisions.

### 4.2 Process

#### Step 1: Launch Architecture Agent

Use the `embedded-architect` agent to analyze requirements and generate architecture artifacts:

```
Launch embedded-architect agent with:
- Input: docs/requirements/user-stories.md
- Output: docs/architecture/
```

The agent produces:
- Architecture overview
- Module decomposition
- HAL boundary definition
- Dependency map
- C4 diagram spec
- ADR specs (3-5 architectural decisions)

#### Step 2: Generate Architecture Diagrams

Use the `drawio-architect` agent to create visual diagrams from the C4 diagram spec:

```
Launch drawio-architect agent with:
- Input: docs/architecture/c4-diagram-spec.md
- Output: docs/architecture/diagrams/
```

#### Step 3: Document Architectural Decisions

Use the `adr-expert` agent to expand ADR specs into full ADR documents:

```
Launch adr-expert agent with:
- Input: docs/architecture/adr-specs.md
- Output: docs/architecture/decisions/
```

### 4.3 Output Artifacts

| Artifact | Location | Description |
|----------|----------|-------------|
| Architecture Overview | `docs/architecture/architecture-overview.md` | System context and quality attributes |
| Module Decomposition | `docs/architecture/module-decomposition.md` | Module list with responsibilities |
| HAL Boundary | `docs/architecture/hal-boundary.md` | Hardware abstraction interfaces |
| Dependency Map | `docs/architecture/dependency-map.md` | Module dependency graph |
| C4 Diagrams | `docs/architecture/diagrams/` | Visual architecture diagrams |
| ADRs | `docs/architecture/decisions/` | Architectural Decision Records |

### 4.4 Gate G3: Architecture Review

**Before proceeding, verify:**

- [ ] Module decomposition covers all user stories
- [ ] HAL boundary cleanly separates hardware dependencies
- [ ] No circular dependencies in dependency map
- [ ] ADRs document key decisions with rationale
- [ ] Diagrams accurately represent the architecture
- [ ] Memory and timing constraints are addressed

**Reviewer:** Technical lead, senior engineer, or architecture review board

---

## 5. Phase 3: Test Planning

### 5.1 Objective

Create comprehensive test plans for each module before any implementation begins. This ensures test-first thinking and complete coverage.

### 5.2 Process

#### Step 1: Launch Test List Creator

Use the `test-list-creator` agent to generate test plans from architecture:

```
Launch test-list-creator agent with:
- Input: docs/architecture/ (all architecture files)
- Input: Module headers (firmware/scaffolding/include/ or firmware/src/)
- Output: docs/test-plan/unit-tests/
```

The agent produces for each module:
- Test plan markdown (test coverage matrix)
- Test declarations file (compilable test stubs)

#### Step 2: Review Test Coverage Matrix

For each module, verify the test plan covers:

| Category | What to Check |
|----------|---------------|
| Expected Behavior | Normal operation, happy path |
| Error Handling | NULL pointers, invalid inputs, fault conditions |
| Corner Cases | Boundary values (min, max, min-1, max+1) |

### 5.3 Output Artifacts

| Artifact | Location | Description |
|----------|----------|-------------|
| Master Test Index | `docs/test-plan/unit-tests/master-index.md` | All tests across all modules |
| Module Test Plan | `docs/test-plan/unit-tests/<module>/test-plan.md` | Coverage matrix for module |
| Test Declarations | `docs/test-plan/unit-tests/<module>/test-declarations.cpp` | Compilable test stubs |

### 5.4 Test Redundancy Analysis

After generating test plans, perform redundancy analysis to eliminate duplicate coverage:

**Principle: One Test Per Code Branch**

Multiple tests hitting the same code path add maintenance burden without coverage value. Identify and consolidate:

| Redundancy Type | Example | Resolution |
|-----------------|---------|------------|
| Same path, different assertion | Sensor fault: test mode, test heater, test event | Consolidate into one test with multiple assertions |
| Getter tests duplicate setter tests | `GetEvents_HeaterOn` duplicates `Update_HeaterOn_RaisesEvent` | Keep setter test, remove getter duplicate |
| Boundary tests same as typical | `temp == threshold` when `temp >= threshold` already tested | Remove redundant boundary test |

**Process:**
1. Map each test to the specific code branch it exercises (file:line)
2. Identify tests that map to the same branch
3. Keep the test that best verifies the behavior; remove redundant tests
4. Document rationale in test plan

**Quality check:** After removing redundant tests, verify 100% branch coverage is maintained.

### 5.5 Gate G4: Test Plan Review

**Before proceeding, verify:**

- [ ] Every public function has expected-behavior tests
- [ ] Every public function has error-handling tests
- [ ] Boundary value analysis is complete for bounded parameters
- [ ] Test naming follows convention: `Function_Scenario_ExpectedResult`
- [ ] Redundancy analysis completed — one test per code branch
- [ ] Total test count is reasonable (not excessive, not sparse)
- [ ] Traceability: tests map back to user story acceptance criteria

**Reviewer:** QA lead, technical lead, or peer engineer

---

## 6. Phase 4: Scaffolding

### 6.1 Objective

Generate compilable code scaffolding (headers, source stubs, build system) that implements the architecture interfaces without business logic.

### 6.2 Process

#### Step 1: Launch Scaffold Engineer

Use the `firmware-scaffold-engineer` agent to generate scaffolding:

```
Launch firmware-scaffold-engineer agent with:
- Input: docs/architecture/ (all architecture files)
- Output: firmware/scaffolding/
```

The agent produces:
- Header files with type definitions and function prototypes
- Source file stubs (functions return stub values)
- Build system files (Makefile, CMakeLists.txt)

#### Step 2: Verify Scaffolding Compiles

Build the scaffolding to verify it compiles without errors:

```bash
docker exec <container> make -C firmware/scaffolding
```

#### Step 3: Copy to Firmware Directory

Once scaffolding is validated, copy it to the firmware source tree:

```bash
cp -r firmware/scaffolding/include/* firmware/src/
cp -r firmware/scaffolding/src/* firmware/src/
```

### 6.3 Output Artifacts

| Artifact | Location | Description |
|----------|----------|-------------|
| Module Headers | `firmware/src/<module>/<module>.h` | Public interfaces |
| Module Sources | `firmware/src/<module>/<module>.c` | Stub implementations |
| Build Files | `firmware/` | Makefile, CMakeLists.txt |

### 6.4 Gate G5: Scaffolding Review

**Before proceeding, verify:**

- [ ] Scaffolding compiles without errors or warnings
- [ ] Header files match architecture (module decomposition, HAL boundary)
- [ ] Type definitions are complete (structs, enums, typedefs)
- [ ] Function signatures match test declarations
- [ ] No business logic in stubs (just return values)

**Reviewer:** Technical lead or peer engineer

---

## 7. Phase 5: Test-Driven Implementation

### 7.1 Objective

Implement each module using strict Test-Driven Development: RED (failing test) → GREEN (minimal implementation) → REFACTOR.

### 7.2 Process

#### Step 1: Select Module Order

Implement modules **top-down** (outside-in TDD):

1. **Application/orchestration modules** — Start with the highest-value user-facing behavior
2. **Middle-layer modules** — Implement as tests require them
3. **HAL modules** — Mock until hardware validation is needed
4. **Utility modules** — Implement when tests demand specific utility behavior

**Why top-down?**
- Keeps focus on delivering user value first
- Tests drive out the interfaces of lower modules (you discover what you actually need)
- Avoids building infrastructure that might not be needed
- Catches integration issues earlier

**Mocking strategy:**
- Mock lower-level dependencies until you need real behavior
- When a mock becomes complex, it's a signal to implement that module
- HAL mocks remain in place for unit tests even after real HAL is implemented

**Practical Considerations:**

| Situation | Approach |
|-----------|----------|
| **Multiple developers** | One developer works top-down on application logic while another works bottom-up on HAL/hardware-facing modules in parallel. This flushes out hardware gotchas early while app development continues. The top-down mocks inform what the HAL interface should look like. |
| **Single developer** | Primarily top-down, but spike low-level modules when needed to verify hardware behavior (timing, peripheral quirks, silicon errata). Return to top-down once hardware assumptions are validated. |
| **Risk-driven spikes** | If there's a risky or uncertain area (unfamiliar peripheral, tight timing constraints, datasheet ambiguities), spike that path early regardless of module order. A "tracer bullet" through the risky path validates assumptions before committing to a design. |
| **Hardware not available** | Top-down with mocks is the only option. When hardware arrives, pause to validate HAL assumptions before continuing. |
| **HAL interface refinement** | The HAL interface designed top-down may need adjustment once you hit real hardware. That's expected — your unit tests catch regressions when you update the interface. |

#### Step 2: TDD Cycle for Each Module

For each module, choose one of these approaches:

**Option A: Automated (Agent)**

Use the `tdd-module-implementer` agent to implement an entire module:

```
Launch tdd-module-implementer agent with:
- Input: docs/test-plan/unit-tests/<module>/test-plan.md
- Input: docs/test-plan/unit-tests/<module>/test-declarations.cpp
- Output: firmware/tests/<module>/test_<module>.cpp
- Output: firmware/src/<module>/<module>.c
```

The agent internally uses the two TDD skills (`tdd-create-failing-test` and `tdd-write-passing-implementation`) for each test in the plan.

**Option B: Manual (Skills)**

Invoke the TDD skills one at a time for finer control:

1. `/tdd-create-failing-test <test-id>` — RED phase: creates one failing test
2. Run tests, verify failure
3. `/tdd-write-passing-implementation <test-name>` — GREEN phase: minimal implementation
4. Run tests, verify all pass
5. Repeat for next test

Use Option B when you want to understand each behavior as it's implemented, or when debugging a tricky module.

**TDD Rules (strictly enforced):**
- ONE test at a time
- Test MUST fail before implementation (RED)
- Write MINIMUM code to pass (GREEN)
- Modify only ONE function per test
- All previous tests must still pass

#### Step 3: Run Static Analysis

After each module is complete, run static analysis in the Docker container:

```bash
# Clang-Tidy
docker exec <container> clang-tidy firmware/src/<module>/<module>.c -- -I firmware/src

# Cppcheck
docker exec <container> cppcheck --enable=all --error-exitcode=1 firmware/src/<module>/
```

Fix any issues before proceeding.

#### Step 4: Verify Coverage

Run coverage report and verify 100% (or near-100%) line/branch coverage:

```bash
docker exec <container> make -f firmware/tests/cpputest.mk coverage
```

#### Step 5: Archive Test Results (For Compliance)

For projects requiring audit evidence (IEC 61508, etc.), archive test results:

```bash
docker exec <container> make -f firmware/tests/cpputest.mk audit
```

This generates a timestamped report in `docs/verification/<module>/` containing:
- Test output (pass/fail)
- Coverage metrics
- Git commit hash
- Build environment info

**When to archive:**
- Before each module code review (G6)
- Before each release (G8)
- After any significant test changes

Archive files should be committed to version control as verification evidence.

#### Step 6: Module Code Review

Submit module for code review before starting next module.

### 7.3 Output Artifacts

| Artifact | Location | Description |
|----------|----------|-------------|
| Test File | `firmware/tests/<module>/test_<module>.cpp` | Implemented unit tests |
| Source File | `firmware/src/<module>/<module>.c` | Implemented module |
| TDD Progress | `docs/test-plan/unit-tests/<module>/tdd-progress.md` | Implementation checklist |
| Coverage Report | `firmware/tests/coverage/` | gcovr HTML report |

### 7.4 Gate G6: Module Code Review

**For each module, before proceeding to the next:**

- [ ] All tests from test plan are implemented and passing
- [ ] Code coverage is ≥95% (ideally 100%)
- [ ] Static analysis passes with no warnings
- [ ] Code follows project coding standards
- [ ] No anti-patterns (over-engineering, anticipated features)
- [ ] TDD progress file is updated
- [ ] Pull request created and approved

**Review Process:**

Submit module changes via **pull request** (PR) to document the review:
- PR description should summarize what the module does and link to the test plan
- CI pipeline must pass before review
- Reviewer comments and approvals are preserved in version control history
- Merge only after approval — this creates an audit trail for traceability

**Reviewer:** Peer engineer or technical lead

### 7.5 Simulation (Where Applicable)

**When to simulate:** After unit tests pass, before code review (Gate G6)

**Which modules benefit from simulation:**

| Module Type | Simulate? | Rationale |
|-------------|-----------|-----------|
| Pure logic modules | No | Unit tests are sufficient |
| HAL modules | Yes | Verify register sequences and peripheral state machines |
| Modules with logical timing dependencies | Yes | Verify sequence/ordering between events |
| Peripheral drivers | Depends | Only if simulator supports the peripheral |

**What simulation catches:**
- Register access sequences (correct order, correct values)
- Peripheral state machine transitions
- Logical race conditions (two modules accessing shared state)
- Interrupt sequencing (ISR fires before/after expected event)
- Protocol sequence violations

**What simulation does NOT catch:**
- Real-time execution timing (μs/ns deadlines)
- Cycle-accurate behavior
- Clock jitter, temperature, and voltage effects
- Silicon errata timing quirks

> **Note:** Most simulators (Renode, QEMU) are **functionally accurate**, not **cycle-accurate**. For real-time timing validation (latency, deadlines, jitter), use HIL testing on actual hardware (Section 8).

> **TODO:** Define project-specific simulation strategy
>
> **Simulation tools to evaluate:**
> - [Renode](https://renode.io/) — Open source, good ARM Cortex-M support
> - QEMU — More general, requires BSP work
> - Vendor simulators (MPLAB Simulator, etc...)
>
> **Action item:** Create `.claude/skills/simulation-runner/SKILL.md` when simulation strategy is defined

---

## 8. Phase 6: Integration & System Testing

### 8.1 Objective

Verify all modules work together correctly and the system meets user story acceptance criteria.

### 8.2 Process

#### Step 1: Integration Testing

Create integration tests that exercise module interactions:

- Test module A calling module B
- Test event propagation across modules
- Test error handling across module boundaries

```
Location: firmware/tests/integration/
```

#### Step 2: System Testing

Verify complete user stories end-to-end:

- Map each user story to system test(s)
- Test acceptance criteria directly
- Include non-functional tests (timing, power, memory)

#### Step 3: System Simulation

**When to run:** After integration tests pass, before HIL testing

**Purpose:** Validate multi-module interactions in simulation before committing to hardware testing (faster iteration, cheaper to run, can inject faults).

**What to test in system simulation:**
- Full use-case scenarios end-to-end
- Multi-peripheral interactions (SPI + DMA + GPIO together)
- Interrupt priority and nesting behavior
- Fault injection scenarios (bus errors, peripheral timeouts)
- Edge cases that are hard to create on real hardware

> **TODO:** Define system simulation test cases in `docs/test-plan/simulation/`

#### Step 4: Hardware-in-the-Loop Testing

**When to run:** After system simulation passes (or after integration tests if simulation is not used)

**Purpose:** Validate real-time timing, actual hardware behavior, and silicon-specific quirks that simulation cannot catch.

**What HIL catches that simulation misses:**

| Category | Examples |
|----------|----------|
| Real-time timing | ISR latency <10μs, control loop at exactly 1kHz |
| Electrical behavior | Signal integrity, noise immunity, ESD |
| Silicon errata | Chip-specific bugs documented in errata sheets |
| Environmental | Temperature effects, voltage margins |
| Peripheral quirks | Undocumented timing requirements, edge cases |

> **TODO:** Define HIL testing strategy (project-specific)
>
> **Suggested milestones for HIL:**
> - After all HAL modules complete — verify hardware interfaces
> - After system simulation — verify real-time behavior on actual hardware
> - Before release — full regression on target hardware
>
> **HIL setup options:**
> - Development board + Python/pytest scripts
> - Custom test fixture with GPIO/ADC stimulus
> - Commercial HIL platform (dSPACE, NI)
>
> **Action item:** Create `docs/test-plan/hil-tests/` directory and HIL test plan when strategy is defined

### 8.3 Output Artifacts

| Artifact | Location | Description |
|----------|----------|-------------|
| Integration Tests | `firmware/tests/integration/` | Cross-module tests |
| System Test Plan | `docs/test-plan/system-tests.md` | User story verification |
| HIL Test Results | `docs/test-plan/hil-tests/results/` | Hardware test evidence |

### 8.4 Gate G7: Integration Review

**Before proceeding to release:**

- [ ] All integration tests pass
- [ ] Each user story has at least one passing system test
- [ ] HIL tests pass on target hardware (if applicable)
- [ ] No regressions in unit tests
- [ ] Performance meets constraints (timing, memory, power)

**Reviewer:** Technical lead, QA lead, or project manager

---

## 9. Phase 7: Release

### 9.1 Objective

Package validated firmware for production with complete documentation and release artifacts.

### 9.2 Process

#### Step 1: Release Candidate Build

Create a tagged release build:

```bash
git tag -a v1.0.0 -m "Release 1.0.0"
docker exec <container> ./scripts/release.sh v1.0.0
```

> **TODO:** Create project release script (`scripts/release.sh` or similar)
>
> The release script should:
> - Build with release/production configuration (optimizations enabled, debug disabled)
> - Generate binary artifacts (.bin, .hex, .elf as needed)
> - Embed version information in the binary
> - Output artifacts to `build/release/`
>
> **Build system options:**
> - CMake: `cmake --build . --config Release`
> - Make: `make -f firmware/Makefile release`
> - Vendor IDE: Command-line build invocation

#### Step 2: Final Verification

- [ ] Release build compiles without warnings
- [ ] All tests pass on release build
- [ ] Binary size within constraints
- [ ] Static analysis clean

#### Step 3: Documentation

Verify all documentation is complete and up-to-date:

- [ ] Architecture documents match implementation
- [ ] ADRs reflect final decisions
- [ ] Test plans show all tests passing
- [ ] API documentation generated (if applicable)

#### Step 4: Release Notes

Create release notes summarizing:
- Features implemented
- Known issues/limitations
- Hardware compatibility
- Upgrade instructions (if applicable)

### 9.3 Output Artifacts

| Artifact | Location | Description |
|----------|----------|-------------|
| Release Binary | `build/release/firmware.bin` | Production binary |
| Release Notes | `docs/releases/v1.0.0.md` | Release documentation |
| Test Report | `docs/releases/v1.0.0-test-report.md` | Test execution evidence |

### 9.4 Gate G8: Release Approval

**Before releasing:**

- [ ] All Gates G1-G7 have been passed
- [ ] Release binary verified on target hardware
- [ ] Documentation complete and accurate
- [ ] Stakeholder sign-off obtained
- [ ] Release notes reviewed

**Reviewer:** Project manager, technical lead, customer (if applicable)

---

## 10. Traceability

### 10.1 Overview

This SDLC maintains bidirectional traceability between requirements, tests, and implementation:

```
Product Brief
     │
     ▼
User Stories (US-001, US-002, ...)
     │
     ├──────────────────────────────────────┐
     ▼                                      ▼
Architecture Modules              Test Plan (TCI-001, TCU-001, ...)
     │                                      │
     ▼                                      ▼
Implementation                    Unit Tests
(firmware/src/<module>/)          (firmware/tests/<module>/)
```

### 10.2 Forward Traceability (Requirements → Tests → Code)

**Question:** "Does this requirement have tests, and is it implemented?"

| From | To | How to Trace |
|------|----|--------------|
| User Story | Test Plan | Test plan references user story ID in coverage matrix |
| Test Plan | Test File | Test declarations match test file TEST() macros |
| Test File | Source | Test exercises specific function in source file |

**Example:**
```
US-003: Temperature control with hysteresis
  └── docs/test-plan/unit-tests/temp_ctrl/test-plan.md (TCU-001 through TCU-034)
        └── firmware/tests/temp_ctrl/test_temp_ctrl.cpp (46 tests)
              └── firmware/src/temp_ctrl/temp_ctrl.c (temp_ctrl_update)
```

### 10.3 Backward Traceability (Code → Tests → Requirements)

**Question:** "Why does this code exist? What requirement drove it?"

| From | To | How to Trace |
|------|----|--------------|
| Source Function | Test File | Function name appears in TEST() that exercises it |
| Test File | Test Plan | TEST() name matches test plan declaration (by test ID) |
| Test Plan | User Story | "User Story" column in test plan links to story ID |

**Example:**
```
temp_ctrl_update() in temp_ctrl.c
  └── TEST(TempCtrl, test_temp_ctrl_update_heater_on) in test_temp_ctrl.cpp
        └── TCU-001 in test-plan.md (User Story: US-003)
              └── US-003: Temperature control with hysteresis
```

### 10.4 Implementing Traceability

**Step 1: Embed User Story IDs in Test Plans**

Add a "User Story" column to each test plan's coverage matrix:

```markdown
| ID | Function | Scenario | Expected Result | User Story |
|----|----------|----------|-----------------|------------|
| TCI-001 | temp_ctrl_init | Valid config | Returns OK | US-003 |
| TCI-007 | temp_ctrl_init | NULL state | Returns error | US-003 |
| TCU-001 | temp_ctrl_update | Below setpoint | Heater on | US-003, US-005 |
```

**Step 2: Reference Story IDs in Git**

Include user story IDs in commit messages and PR descriptions:

```
feat(temp_ctrl): implement hysteresis control

Implements bang-bang temperature control with configurable hysteresis.

User Stories: US-003, US-005
Tests: TCU-001 through TCU-012
```

**Step 3: Maintain Traceability Matrix**

Create `docs/requirements/traceability-matrix.md` that aggregates all links:

```markdown
| User Story | Description | Modules | Test IDs | Status |
|------------|-------------|---------|----------|--------|
| US-001 | System initialization | bsp, hal | TCI-001..006 | Complete |
| US-003 | Temperature control | temp_ctrl | TCI-*, TCU-* | Complete |
| US-005 | Heater safety limits | temp_ctrl | TCU-017..022 | Complete |
```

### 10.5 Requirements Traceability Matrix (RTM)

Maintain a formal RTM at `docs/requirements/traceability-matrix.md` containing:

1. **Source Requirements → User Stories** — Which source paragraphs map to which stories
2. **User Stories → Test Cases** — Test IDs covering each story
3. **Test Cases → Source Code** — Function and line numbers exercised by each test
4. **Backward Index** — Source code → tests → stories (for "why does this exist?")
5. **Coverage Analysis** — Stories with/without tests, orphan detection
6. **Verification Statement** — Attestation that traceability is complete

**Update the RTM when:**
- New user stories are added
- New tests are implemented
- Code changes affect test coverage
- Modules are completed

### 10.6 Traceability Tools

Choose based on project needs:

| Approach | Tool/Method | Best For |
|----------|-------------|----------|
| **Lightweight** | Markdown matrix + git grep | Small teams, simple projects |
| **Integrated** | Jira + Xray, Azure DevOps | Teams already using ALM tools |
| **Regulated** | DOORS, Jama, Polarion, Helix RM | Safety-critical, certification required |

**Lightweight approach (recommended for most projects):**
- Test plans link tests → user stories (embedded in markdown)
- Git commits/PRs reference user story IDs
- Traceability matrix maintained manually or generated from test plans
- Use `git log --grep="US-003"` to find all commits for a story

> **TODO:** Create automated traceability report generator
>
> **Suggested approach:**
> - Parse test plan markdown for test IDs and user story references
> - Parse test files for TEST() macro counts
> - Cross-reference with user stories document
> - Generate HTML/markdown report showing coverage per user story
>
> **Action item:** Create `.claude/skills/traceability-report/SKILL.md`

---

## 11. CI/CD Integration

### 11.1 Pipeline Triggers

| Trigger | Checks Run |
|---------|------------|
| Push to feature branch | Compile check (fast feedback) |
| Pull Request | Full pipeline (build + tests + static analysis + coverage) |
| Merge to main | Full pipeline + generate release artifacts |
| Tag (release) | Full pipeline + HIL tests (if configured) |

> **Note:** CI/CD pipelines trigger on pushes to the remote repository, not local commits. For fast local feedback before pushing, consider using git pre-commit hooks to run compile checks or linting locally.

### 11.2 CI Pipeline Stages

```yaml
stages:
  - build
  - test
  - analyze
  - report

build:
  script:
    - make -f firmware/Makefile all
  artifacts:
    - firmware.elf

test:
  script:
    - make -f firmware/tests/cpputest.mk test
  artifacts:
    - test-results.xml

analyze:
  script:
    - clang-tidy firmware/src/**/*.c -- -I firmware/src
    - cppcheck --enable=all --error-exitcode=1 firmware/src/
  allow_failure: false

coverage:
  script:
    - make -f firmware/tests/cpputest.mk coverage
    - gcovr --fail-under-line 95
  artifacts:
    - coverage/
```

### 11.3 Quality Gates in CI

| Check | Threshold | Action on Failure |
|-------|-----------|-------------------|
| Compilation | 0 errors, 0 warnings | Block merge |
| Unit Tests | 100% pass | Block merge |
| Static Analysis | 0 errors | Block merge |
| Line Coverage | ≥95% | Block merge |
| Branch Coverage | ≥90% | Warning (review required) |

### 11.4 When to Run What

| Development Phase | CI Checks |
|-------------------|-----------|
| During TDD (local) | Unit tests for current module |
| Before PR | Full test suite locally |
| On PR | Automated full pipeline |
| Before merge | Pipeline passed + code review approved |
| After merge | Same pipeline (verify merge didn't break) |

---

## 12. Adapting for Safety-Critical Projects

### 12.1 Overview

For projects requiring functional safety certification (medical, automotive, industrial), this SDLC can be adapted to meet standards like:

- **IEC 61508** — Functional safety (general)
- **IEC 62304** — Medical device software
- **ISO 26262** — Automotive functional safety

**Recommendation:** Use **IEC 61508** as the base template since it's the parent standard from which others derive. Its V-model aligns naturally with this SDLC.

### 12.2 IEC 61508 Traceability Requirements

IEC 61508 Part 3 (Software) requires bidirectional traceability at all SIL levels:

| Traceability Link | SIL 1 | SIL 2 | SIL 3/4 | How We Satisfy |
|-------------------|-------|-------|---------|----------------|
| Safety requirements → Software requirements | HR | HR | HR | RTM Section 2 |
| Software requirements → Architecture | HR | HR | HR | RTM Section 3 |
| Architecture → Design | HR | HR | HR | Module decomposition → source |
| Design → Implementation | HR | HR | HR | RTM Section 4 (test→code) |
| Implementation → Test cases | HR | HR | HR | RTM Section 4 |
| Test cases → Test results | HR | HR | HR | Test reports + coverage |

**HR = Highly Recommended**

**RTM Location:** `docs/requirements/traceability-matrix.md`

### 12.3 Additional Requirements for Safety-Critical

| SDLC Phase | Additional Safety Requirements |
|------------|-------------------------------|
| Requirements | Hazard analysis, safety requirements identified |
| Architecture | Safety Integrity Level (SIL) assigned per module |
| Test Planning | Tests mapped to safety requirements, MC/DC coverage for high SIL |
| Implementation | MISRA-C compliance, defensive coding |
| Verification | Independent verification, formal review records |
| Release | Safety case, certification evidence package |

### 12.4 Documentation Additions

For safety-critical projects, add:

| Document | Purpose | Location |
|----------|---------|----------|
| Requirements Traceability Matrix | Bidirectional traceability | `docs/requirements/traceability-matrix.md` |
| Hazard Analysis | Identify potential hazards and mitigations | `docs/safety/hazard-analysis.md` |
| Safety Requirements Spec | Safety-specific requirements | `docs/safety/safety-requirements.md` |
| Software Safety Plan | How safety will be achieved | `docs/safety/software-safety-plan.md` |
| Verification Report | Evidence of verification activities | `docs/verification/verification-report.md` |
| Review Records | Evidence of design/code/test reviews | `docs/reviews/` |
| Safety Case | Argument that system is acceptably safe | `docs/safety/safety-case.md` |

### 12.5 IEC 61508 Checklist

Use this checklist to assess readiness for IEC 61508 compliance:

**Traceability (Table A.1, Table B.1):**
- [ ] Requirements → Tests → Code bidirectionally linked
- [ ] RTM maintained and version controlled
- [ ] No orphan tests (tests without requirements)
- [ ] No orphan code (code without tests)

**Documentation (Table A.4):**
- [ ] Software requirements specification exists
- [ ] Software architecture description exists
- [ ] Module design descriptions exist
- [ ] Test specifications exist with expected results

**Verification (Table A.5):**
- [ ] Test results archived with version info
- [ ] Coverage reports generated and reviewed
- [ ] Static analysis performed and documented
- [ ] Review records exist for design, code, tests

**Configuration Management (Table A.8):**
- [ ] All artifacts under version control
- [ ] Change history maintained
- [ ] Build environment documented and reproducible

> **TODO:** Create safety-critical project template
>
> **Action items:**
> - Create `docs/templates/safety/` with safety document templates
> - Create `.claude/skills/safety-analysis/SKILL.md` for hazard identification
> - Add MISRA-C checking to static analysis pipeline
> - Define MC/DC coverage requirements for high-SIL modules

---

## Appendix A: Templates

### A.1 Product Requirements Brief Template

```markdown
# Product Requirements Brief

## Document Control
- **Project:** [Project Name]
- **Version:** 1.0
- **Date:** YYYY-MM-DD
- **Author:** [Name]
- **Status:** Draft | Review | Approved

---

## 1. Problem Statement

### 1.1 Background
[What is the context? Why does this project exist?]

### 1.2 Problem
[What specific problem does this firmware solve?]

### 1.3 Users
[Who will use this system? What are their needs?]

---

## 2. Target Hardware

### 2.1 MCU/Processor
- **Family:** [e.g., ARM Cortex-M4]
- **Part Number:** [e.g., SAMD21J18A]
- **Clock Speed:** [e.g., 48 MHz]

### 2.2 Memory
- **Flash:** [e.g., 1 MB]
- **RAM:** [e.g., 192 KB]
- **External:** [e.g., None / 8 MB SDRAM]

### 2.3 Peripherals Required
- [ ] GPIO
- [ ] UART
- [ ] SPI
- [ ] I2C
- [ ] ADC
- [ ] Timers
- [ ] DMA
- [ ] Other: [specify]

### 2.4 Development Board
[e.g., SAMD21 Xplained Pro, custom board]

---

## 3. Key Features

### 3.1 Must Have (P1)
1. [Feature 1]
2. [Feature 2]
3. [Feature 3]

### 3.2 Should Have (P2)
1. [Feature 4]
2. [Feature 5]

### 3.3 Nice to Have (P3)
1. [Feature 6]

---

## 4. Constraints

### 4.1 Power
- **Budget:** [e.g., 50 mW average, 200 mW peak]
- **Sleep modes required:** [Yes/No]

### 4.2 Timing
- **Control loop:** [e.g., 1 kHz update rate]
- **Response time:** [e.g., <10 ms from input to output]
- **Boot time:** [e.g., <500 ms to operational]

### 4.3 Memory
- **Flash usage:** [e.g., <256 KB]
- **RAM usage:** [e.g., <64 KB]

### 4.4 Other Constraints
- [Regulatory requirements]
- [Environmental conditions]
- [Cost targets]

---

## 5. Success Criteria

The project is successful when:

1. [ ] [Measurable criterion 1]
2. [ ] [Measurable criterion 2]
3. [ ] [Measurable criterion 3]
4. [ ] All unit tests pass with ≥95% coverage
5. [ ] System operates within power budget
6. [ ] No critical or major bugs in release

---

## 6. Milestones and Timeline

### 6.1 Key Milestones

| Milestone | Target Date | Description |
|-----------|-------------|-------------|
| M1: Requirements Complete | YYYY-MM-DD | User stories approved |
| M2: Architecture Review | YYYY-MM-DD | Architecture and ADRs approved |
| M3: Alpha Release | YYYY-MM-DD | Core features implemented |
| M4: Production Release | YYYY-MM-DD | Final release |

### 6.2 Customer Commitments

| Commitment | Date | Notes |
|------------|------|-------|
| [Demo/pilot/delivery] | YYYY-MM-DD | [Details] |

### 6.3 External Dependencies

| Dependency | Owner | Due Date | Impact if Missed |
|------------|-------|----------|------------------|
| [Hardware/API/etc.] | [Who] | YYYY-MM-DD | [Impact] |

---

## 7. Out of Scope

The following are explicitly NOT part of this project:
- [Item 1]
- [Item 2]

---

## Approval

| Role | Name | Date | Signature |
|------|------|------|-----------|
| Project Sponsor | | | |
| Technical Lead | | | |
| Customer Rep | | | |
```

### A.2 User Story Template

```markdown
## US-XXX: [Title]

**Priority:** P1 | P2 | P3
**Story Points:** [estimate]

### Story
As a [role],
I want [capability],
so that [benefit].

### Acceptance Criteria
- [ ] AC1: [Testable criterion]
- [ ] AC2: [Testable criterion]
- [ ] AC3: [Testable criterion]

### Technical Notes
[Any implementation hints or constraints]

### Dependencies
- Depends on: [US-YYY, US-ZZZ]
- Blocks: [US-AAA]

### Traceability
- Requirement: REQ-XXX
- Test Plan: [link when created]
```

### A.3 Configuration Management Plan Template

```markdown
# Configuration Management Plan

**Project:** [Project Name]
**Version:** 1.0
**Date:** YYYY-MM-DD

---

## 1. Version Control

| Item | Value |
|------|-------|
| Tool | Git |
| Repository | [URL or path] |
| Hosting | GitHub / GitLab / Bitbucket / Self-hosted |

**Controlled Items:**
- [ ] Source code (`firmware/src/`)
- [ ] Unit tests (`firmware/tests/`)
- [ ] Documentation (`docs/`)
- [ ] Build configuration (Makefiles, CMakeLists.txt)
- [ ] CI/CD configuration (`.github/`, `.gitlab-ci.yml`)

## 2. Branching Strategy

| Branch | Purpose | Protection |
|--------|---------|------------|
| `main` | Production-ready code | Protected: requires PR + review |
| `develop` | Integration branch (optional) | Protected: requires PR |
| `feature/*` | Feature development | None |
| `bugfix/*` | Bug fixes | None |
| `release/*` | Release candidates | Protected |

## 3. Change Control Process

### 3.1 Making Changes
1. Create feature branch from `main` (or `develop`)
2. Make changes with atomic commits
3. Push branch and create Pull Request
4. CI pipeline must pass
5. Reviewer must approve
6. Merge to target branch

### 3.2 Commit Message Format
```
<type>(<scope>): <subject>

<body>

User Stories: US-XXX, US-YYY
```

Types: `feat`, `fix`, `docs`, `test`, `refactor`, `chore`

### 3.3 Pull Request Requirements
- [ ] CI pipeline passes (build, test, static analysis)
- [ ] Code coverage ≥95%
- [ ] At least 1 reviewer approval
- [ ] No unresolved comments
- [ ] Linked to user story or issue

## 4. Release Management

### 4.1 Versioning
Semantic versioning: `MAJOR.MINOR.PATCH`
- MAJOR: Breaking changes
- MINOR: New features, backward compatible
- PATCH: Bug fixes, backward compatible

### 4.2 Release Process
1. Create release branch `release/vX.Y.Z` from `main`
2. Update version in source files
3. Run full test suite including HIL (if applicable)
4. Create signed tag `vX.Y.Z`
5. Generate release artifacts
6. Create release notes in `docs/releases/vX.Y.Z.md`
7. Merge to `main` and delete release branch

### 4.3 Release Artifacts
| Artifact | Location | Description |
|----------|----------|-------------|
| Binary | `build/release/` | Production firmware |
| Test Results | `docs/verification/` | Archived test output |
| Release Notes | `docs/releases/` | Change summary |

## 5. Build Environment

### 5.1 Docker Container
| Item | Value |
|------|-------|
| Image | [image:tag] |
| Registry | [Docker Hub / private registry] |

### 5.2 Tool Versions
| Tool | Version | Purpose |
|------|---------|---------|
| GCC ARM | X.Y.Z | Target compiler |
| CppUTest | X.Y | Unit test framework |
| gcov/gcovr | X.Y | Coverage reporting |
| Clang-Tidy | X.Y | Static analysis |

### 5.3 Reproducibility
The same Docker image used locally MUST be used in CI to ensure consistent results.

## 6. Backup and Recovery

| Item | Backup Method | Recovery |
|------|---------------|----------|
| Repository | Git hosting provider | Clone from remote |
| CI/CD config | In repository | Restore from Git |
| Build artifacts | CI artifact storage | Re-run release build |

---

## Approval

| Role | Name | Date |
|------|------|------|
| Technical Lead | | |
| Project Manager | | |
```

### A.4 Review Record Template

```markdown
# Review Record

**Gate:** G[1-8] — [Gate Name]
**Date:** YYYY-MM-DD
**Artifact(s) Reviewed:** [List of documents/files]

---

## Attendees

| Name | Role |
|------|------|
| [Name] | Reviewer |
| [Name] | Author |
| [Name] | Observer |

---

## Checklist

[Copy checklist from SDLC manual for this gate]

- [ ] Item 1
- [ ] Item 2
- [ ] Item 3

---

## Findings

| ID | Finding | Severity | Resolution | Status |
|----|---------|----------|------------|--------|
| F1 | [Description] | Major/Minor/Comment | [How resolved] | Closed |
| F2 | [Description] | Major/Minor/Comment | [How resolved] | Open |

**Severity definitions:**
- **Major:** Must be resolved before approval
- **Minor:** Should be resolved, does not block approval
- **Comment:** Suggestion for improvement

---

## Outcome

- [ ] **Approved** — All checklist items satisfied, no open Major findings
- [ ] **Approved with conditions** — Minor findings to be resolved before next gate
- [ ] **Rejected** — Major findings require re-review after resolution

---

## Approval Signatures

| Role | Name | Signature | Date |
|------|------|-----------|------|
| Lead Reviewer | | | |
| Technical Lead | | | |
```

### A.5 Test Results Archive Template

```markdown
# Test Results Archive

**Module:** [module_name]
**Date:** YYYY-MM-DD
**Git Commit:** [full SHA]
**Git Tag:** [if release]

---

## Test Execution

| Item | Value |
|------|-------|
| Test Framework | CppUTest |
| Build Config | Debug / Release |
| Platform | Host (x86_64) / Target (ARM) |
| Docker Image | [image:tag] |

---

## Results Summary

```
[Paste test output here]

OK (38 tests, 38 ran, 81 checks, 0 ignored, 0 filtered out, 0 ms)
```

---

## Code Coverage

| Metric | Value | Threshold | Status |
|--------|-------|-----------|--------|
| Line Coverage | XX.XX% | ≥95% | PASS/FAIL |
| Branch Coverage | XX.XX% | ≥90% | PASS/FAIL |
| Function Coverage | XX.XX% | 100% | PASS/FAIL |

---

## Static Analysis

| Tool | Errors | Warnings | Status |
|------|--------|----------|--------|
| Clang-Tidy | 0 | 0 | PASS |
| Cppcheck | 0 | 0 | PASS |

---

## Verification Statement

All tests passed. Coverage thresholds met. Static analysis clean.

**Verified by:** [Name]
**Date:** YYYY-MM-DD
```

---

## Appendix B: Tool Reference

### B.1 AI Agents

| Agent | Purpose | When to Use |
|-------|---------|-------------|
| `embedded-architect` | Generate architecture from requirements | Phase 2 |
| `drawio-architect` | Create architecture diagrams | Phase 2 |
| `adr-expert` | Document architectural decisions | Phase 2 |
| `test-list-creator` | Generate test plans from architecture | Phase 3 |
| `firmware-scaffold-engineer` | Generate code scaffolding | Phase 4 |
| `tdd-module-implementer` | Implement modules via TDD | Phase 5 |

### B.2 AI Skills

| Skill | Purpose | When to Use |
|-------|---------|-------------|
| `/tdd-create-failing-test` | RED phase — create one failing test | Manual TDD |
| `/tdd-write-passing-implementation` | GREEN phase — minimal implementation | Manual TDD |
| `/coding-standards` | Enforce project coding conventions | Before writing code |

### B.3 Build Tools

| Tool | Command | Purpose |
|------|---------|---------|
| CppUTest | `make -f cpputest.mk test` | Run unit tests |
| gcovr | `make -f cpputest.mk coverage` | Generate coverage report |
| Audit | `make -f cpputest.mk audit MODULE=<name>` | Generate compliance test archive |
| Clang-Tidy | `clang-tidy <file>` | Static analysis |
| Cppcheck | `cppcheck --enable=all <dir>` | Static analysis |

---

## Revision History

| Version | Date | Author | Changes |
|---------|------|--------|---------|
| 1.0 | 2026-09-22 | [Author] | Initial release |
