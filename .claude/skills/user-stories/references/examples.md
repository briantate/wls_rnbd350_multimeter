# Worked Examples

Three worked examples that show the bar for output quality. Read these
when you need a reference for how implied personas, traceability, layer
tagging, and parent–child decomposition are handled.

---

## Example 1 — Short brief (interpretation-heavy, single layer)

### Input

> **Password reset**
>
> Users sometimes forget their password. We want them to be able to reset
> it themselves without contacting support. Send an email with a link
> when they ask. The link should expire so it isn't valid forever. Don't
> let people use the same link twice.

### Notes on what this input forces

- Persona is not named — only "users". Implied: an account holder who has
  lost their password. Label as an inferred persona.
- Benefit is stated indirectly ("without contacting support") — usable.
- Three rules are stated (email link, expiry, single-use). These are AC,
  not stories.
- "Expire" has no duration. Choose a domain-standard default (24 hours)
  and flag it as an assumption.
- Single-layer input (Product). One natural user-facing story; no
  enabler decomposition needed at this scope.

### Output excerpt

```markdown
# User Stories — Password Reset

**Source:** Password reset brief (provided inline)
**Generated:** 2026-04-26
**Input layer:** Product brief

## Summary

A self-service flow that lets an account holder regain access to their
account by requesting a reset link sent to their email, without contacting
support.

## Personas

- **Account holder** — a registered user who has lost or forgotten their
  password. (Inferred — see Assumptions.)

## Story map

- **001 — Reset password via emailed link** _[User-facing, Product]_

## Definition of Ready

- [ ] AC reviewed and unambiguous.
- [ ] Open question on unknown-email behavior resolved.
- [ ] Estimate agreed.

## Definition of Done

- [ ] AC pass against a staging deployment.
- [ ] Email deliverability verified end-to-end.
- [ ] Security review completed (token entropy, link binding, replay).
- [ ] Telemetry on send, open, and consume rates wired up.
- [ ] Code review approved; CI green; merged.

## Backlog

### Epic E1 — Self-service password recovery

#### 001 — Reset password via emailed link

> As an account holder,
> I want to request a password reset link by email,
> so that I can regain access to my account without contacting support.

- **Type:** User-facing
- **Layer:** Product
- **Parent:** —
- **Persona:** Account holder
- **Priority:** Must
- **Estimate:** ~5 days
- **Depends on:** —
- **Status:** Draft
- **Source:** Password reset brief, paragraph 1.

##### Acceptance criteria

###### Scenario: Successful reset request

```gherkin
Given an account holder has an active account
When  they request a password reset for their email address
Then  a reset email is sent to that address within 60 seconds
And   the email contains a single-use link
```

###### Scenario: Reset link is single-use

```gherkin
Given an account holder has used a reset link to set a new password
When  they attempt to use the same link again
Then  the link is rejected and the user is shown a "link already used" message
```

###### Additional rules

- The reset link expires 24 hours after it is issued.
- An expired link is rejected with a "link expired" message.
```

### What this example demonstrates

- Single-layer Product input → no parent-child decomposition needed.
- Inferred persona named in the glossary and called out in Assumptions.
- Each scenario has its own subheading and its own fenced block.
- Mixed AC: GWT for behaviors, checklist for simple rules.
- Backlog-level DoD, separate from per-story AC.

---

## Example 2 — Long mixed spec (triage-heavy, mixed layers)

### Input shape

A 25-page module spec for a temperature controller. Sections include:
hardware overview, sensor inventory, control loop math, alarm thresholds,
operator UI mockups, maintenance procedures, certification notes, and
glossary.

### Notes on what this input forces

- Multiple actors implied: **operator**, **maintenance technician**,
  **safety officer**. The spec writes "user" generically — split.
- Hardware overview, control math, and certification notes are
  *background*, not requirements. They inform AC and DoD but do not
  generate stories.
- Alarm thresholds and maintenance procedures are dense with rules —
  candidates for checklist AC.
- Operator UI mockups are prescriptive UI; capture *intent* in the "I
  want" clauses, not the layout.
- Mixed-layer input: user-facing operator stories at Product layer,
  enabler stories at Component layer. Tag both.

### Output excerpt (story map only)

```markdown
## Story map

- **001 — Operator monitors current process temperature** _[User-facing, Product]_
  - 010 — Read process temperature from configured sensor _[Enabler, Component]_
  - 011 — Render numeric display with unit and trend indicator _[Enabler, Component]_
- **002 — Operator adjusts setpoint within allowed range** _[User-facing, Product]_
  - 012 — Validate setpoint against operator range and reject out-of-range _[Enabler, Component]_
  - 013 — Persist setpoint across restart _[Enabler, Component]_
- **003 — Operator acknowledges an active alarm** _[User-facing, Product]_
  - 014 — Map operator acknowledgement to alarm-state transition _[Enabler, Component]_
- **004 — Maintenance technician calibrates a sensor offline** _[User-facing, Product]_
  - 015 — Place controller in calibration mode (suspends control output) _[Enabler, Component]_
  - 016 — Record calibration offset for a single sensor _[Enabler, Component]_
- **005 — Safety officer reviews alarm history** _[User-facing, Product]_
  - 017 — Filter alarm history by date range and severity _[Enabler, Component]_
  - 018 — Export alarm history in compliance-ready format _[Enabler, Component]_
- _Top-level technical/NFR stories:_
  - 019 — Worst-case control-loop execution time within budget _[Technical, Component]_
  - 020 — Module is reusable across hardware targets via HAL _[Technical, Component]_
```

### What this example demonstrates

- One "user" in the source becomes three named personas with distinct
  needs, each justified in Assumptions.
- Each user-facing parent has explicit enabler children. Implementation
  cost is no longer hidden.
- Top-level technical/NFR stories (timing, portability) are listed
  separately at the bottom of the map.

---

## Example 3 — Embedded user-facing-to-enabler decomposition

This is the canonical demonstration of the layered story concept.
Read this whenever the input is at component or module layer, or
whenever a user-facing story is suspected to hide non-trivial
implementation cost.

### Input

> When an operator turns on the device using the front-panel power
> button, the device powers up, displays a splash screen, runs a
> self-test, and is ready to accept setpoint adjustments.

### Notes on what this input forces

- One sentence; one user-facing outcome. Persona is the operator.
- The implementation iceberg is large: power sequencing, peripheral
  init order, default-state load, watchdog start, splash UI, self-test,
  fault path, transition to ready state.
- Without decomposition, this story is "small" on the card and three
  months in the codebase. The decomposition pass exposes the actual
  scope.

### Output excerpt

```markdown
## Story map

- **001 — Operator powers on the device and reaches ready state** _[User-facing, Product]_
  - 002 — Initialize MCU clocks and core peripherals on boot _[Enabler, Module]_
  - 003 — Bring up board peripherals in dependency order _[Enabler, Component]_
  - 004 — Load default control state from non-volatile storage _[Enabler, Component]_
  - 005 — Start the watchdog and define the kick contract _[Enabler, Component]_
  - 006 — Render the splash screen during self-test _[Enabler, Component]_
  - 007 — Run power-on self-test and surface pass/fail _[Enabler, Component]_
  - 008 — Transition to ready state once self-test passes _[Enabler, Component]_
  - 009 — Halt in a safe state if self-test fails _[Enabler, Component]_

## Backlog

#### 001 — Operator powers on the device and reaches ready state

> As an operator,
> I want the device to be ready to accept setpoint adjustments shortly
> after I press the power button,
> so that I can begin a run without troubleshooting the device's
> startup behavior.

- **Type:** User-facing
- **Layer:** Product
- **Parent:** —
- **Persona:** Operator
- **Priority:** Must
- **Estimate:** Epic — see children for sizing
- **Depends on:** —
- **Status:** Draft
- **Source:** Power-on brief.

##### Acceptance criteria

###### Scenario: Healthy power-on reaches ready state

```gherkin
Given the device is powered off
When  the operator presses the front-panel power button
Then  within the configured ready time the device displays the
      ready-to-accept-input state
And   the splash screen and self-test results are shown during the
      transition
```

###### Scenario: Self-test failure halts in safe state

```gherkin
Given the device is powering on
When  the power-on self-test reports a failure
Then  the device displays the failure code and remains in a safe
      state with no control outputs asserted
```

#### 002 — Initialize MCU clocks and core peripherals on boot

> As the firmware developer integrating the boot sequence,
> I want MCU clocks and core peripherals brought up in a defined order
> before any application code runs,
> so that downstream initialization can rely on a predictable system
> clock and interrupt configuration.

- **Type:** Enabler
- **Layer:** Module
- **Parent:** 001
- **Persona:** Firmware developer integrating the boot sequence
- **Priority:** Must
- **Estimate:** ~3 days
- **Depends on:** —
- **Status:** Draft
- **Source:** Power-on brief, "powers up". Concrete startup sequence
  inferred (see Assumptions).

##### Acceptance criteria

###### Scenario: Clocks and core peripherals are configured before main()

```gherkin
Given a cold reset
When  the boot sequence runs
Then  system clocks are configured to the target frequency before
      main() is entered
And   NVIC priorities and core peripherals (SysTick, GPIO clocks,
      I2C clocks) are configured before any application init runs
```

###### Additional rules

- The boot sequence does not enable interrupts until clocks are
  stable.
- Worst-case time from reset to main() is measured and recorded.

#### 005 — Start the watchdog and define the kick contract

> As the firmware developer integrating the boot sequence,
> I want the watchdog started early in boot with a documented kick
> contract,
> so that a hung initialization triggers a recoverable reset rather
> than an indefinite hang.

- **Type:** Enabler
- **Layer:** Component
- **Parent:** 001
- **Persona:** Firmware developer integrating the boot sequence
- **Priority:** Must
- **Estimate:** ~2 days
- **Depends on:** 002
- **Status:** Draft
- **Source:** Inferred from "ready to accept setpoint adjustments"
  (implies a healthy, supervised runtime). Flagged in Assumptions.

##### Acceptance criteria

###### Scenario: Watchdog reset on hung init

```gherkin
Given the device is powering on
When  any initialization step blocks for longer than the watchdog
      timeout without kicking
Then  the watchdog resets the MCU
And   on the second consecutive reset, the device halts in a safe
      diagnostic state instead of looping forever
```

###### Additional rules

- The kick contract (where in the loop, at what cadence) is documented
  in the public API.

## Assumptions

- **Watchdog and kick contract** — the source did not mention a
  watchdog. Inferred because "ready" implies a supervised runtime;
  embedded standard practice is to enable a watchdog before any
  long-running init. Confirm with stakeholders.
- **Self-test scope** — the source said "self-test" but did not list
  what is tested. Assumed: sensor presence/ID, heater driver loopback,
  RAM check. Confirm.

## Open questions

- **What is the configured ready time?** — blocks **001**. Determines
  the timing budget for all enabler children.
- **Self-test failure recovery: latched halt or auto-retry?** —
  blocks **009**. Affects whether the device can come up after a
  transient sensor fault.
```

### What this example demonstrates

- A one-line user-facing story produces eight enabler children in the
  story map. The iceberg is now visible.
- The user-facing parent has its own AC at the operator level
  (observable behavior, ready time, failure path). It is not "done"
  until its children are done; its estimate is **Epic — see children**.
- Each enabler child has a *technical persona* ("firmware developer
  integrating the boot sequence") and an *outcome relevant to that
  persona* ("downstream init can rely on a predictable system clock").
  These are legitimate stories, not "fake user desires" — they are
  classified Enabler.
- Enabler children that aren't directly traceable to the source
  (watchdog, self-test scope) are written but called out in
  Assumptions, not silently fabricated.
- The dependency graph is captured via `Depends on` (e.g., 005 depends
  on 002) so sprint planning can sequence work correctly.
