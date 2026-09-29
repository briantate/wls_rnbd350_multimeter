# Worked Example

A small backlog → SRS + RTM. Demonstrates AC-to-EARS mapping, type
classification, traceability, and how assumptions and open questions
flow from the input.

## Input — story excerpt (from `docs/stories/`)

```markdown
# 008 — Enter safe state on over-temperature

> As an embedded firmware developer integrating the controller,
> I want the controller to force the heater off and latch a fault when
> temperature exceeds the configured over-temperature threshold,
> so that the integrating application cannot drive the regulated
> environment past a thermally unsafe limit.

- **Type:** Enabler
- **Layer:** Component
- **Parent:** 001
- **Priority:** Must
- **Source:** Description ¶3.

## Acceptance criteria

### Scenario: Over-temperature detected during update

\`\`\`gherkin
Given an initialized controller with over-temperature threshold T_max
When  an update is invoked with a temperature reading at or above T_max
Then  the heater output is de-asserted on return
And   an over-temperature fault event is raised
And   the controller enters its safe state and remains there until the
      fault is acknowledged or the controller is re-initialized
\`\`\`

### Additional rules

- The over-temperature threshold is sourced from the configuration
  table; it is not hard-coded in the module.
```

## Concern extraction (working notes)

Concerns lifted from the story above:

1. On detection of `temp ≥ T_max` during update, heater is de-asserted
   before update returns.
2. On detection of `temp ≥ T_max` during update, an over-temperature
   fault event is raised.
3. Once in over-temperature safe state, controller remains there
   across subsequent updates until acknowledged or re-initialized.
4. The `T_max` value is sourced from the configuration table.
5. (Implied by parent story 001) Safe state means heater de-asserted.
6. (DoD-implied) WCET of update including the fault-handling path is
   bounded.
7. (DoD-implied) The fault behavior is verified by replay test.

Concerns 1–3 are **Functional** (shall observable behaviors).
Concern 1 also has a **Performance** angle (must complete before
update returns — quantify if a budget exists).
Concern 4 is **Functional** (configurability is a feature) and is
already covered by story 014 — note as cross-reference, do not
duplicate.
Concern 6 is **Performance** (cross-cutting, parent = product-level
or DoD).
Concern 7 belongs in the SRS verification section / RTM, not as a
requirement.

The "latch until acknowledged or re-initialized" behavior is
**Safety** in domains with a hazard analysis behind it; in this
backlog it traces only to a story (no ISO 26262 / IEC 61508 input was
supplied), so classify as **Functional** with a note in Open
Questions that the safety-integrity level is unassigned.

## Output excerpt — SRS

```markdown
### 3.1 Functional

#### FR-014. Over-temperature triggers heater de-assertion

**Statement.** When an update is invoked with a temperature reading at
or above the configured over-temperature threshold, the temperature
controller shall de-assert the heater output before the update returns.

- **Type:** Functional
- **Verification:** Test
- **Priority:** Must
- **Parent:** Story 008 / AC scenario "Over-temperature detected
  during update", line 1.

#### FR-015. Over-temperature raises a fault event

**Statement.** When an update is invoked with a temperature reading at
or above the configured over-temperature threshold, the temperature
controller shall raise an over-temperature fault event observable
through the public query interface.

- **Type:** Functional
- **Verification:** Test
- **Priority:** Must
- **Parent:** Story 008 / AC scenario, line 2.

#### FR-016. Over-temperature safe state is latched

**Statement.** While the temperature controller is in over-temperature
safe state, the temperature controller shall hold the heater output
de-asserted until the fault is acknowledged or the controller is
re-initialized.

- **Type:** Functional
- **Verification:** Test
- **Priority:** Must
- **Parent:** Story 008 / AC scenario, line 3.
- **Notes:** The "acknowledged or re-initialized" semantics is taken
  from the backlog's Assumptions; see Open Questions §6 — safety
  integrity level not yet assigned.

### 3.2 Performance

#### PR-002. Update execution time is bounded

**Statement.** The temperature controller's `update` operation shall
complete within 5 ms on the reference target across all input paths,
including fault-handling paths.

- **Type:** Performance
- **Verification:** Test (instrumented)
- **Priority:** Must
- **Parent:** Story 004 DoD ("WCET measured and within sample period").
- **Notes:** 5 ms threshold inferred from a 10 Hz sample period; flagged
  in Assumptions.

### 3.9 Safety

*(empty — no hazard analysis or safety standard supplied with this
backlog. See Open Questions §6.)*
```

## Output excerpt — RTM

```markdown
| ReqID  | Type | Statement (summary)                              | Parent           | Verification | Test case   | Priority | Status | Integrity |
|--------|------|--------------------------------------------------|------------------|--------------|-------------|----------|--------|-----------|
| FR-014 | F    | Heater de-asserts on over-temperature            | Story 008 AC L1  | Test         | TC-OT-001   | Must     | Draft  | —         |
| FR-015 | F    | Over-temperature event raised                    | Story 008 AC L2  | Test         | TC-OT-002   | Must     | Draft  | —         |
| FR-016 | F    | Over-temperature safe state latched              | Story 008 AC L3  | Test         | TC-OT-003   | Must     | Draft  | —         |
| PR-002 | P    | Update completes within 5 ms (WCET)              | Story 004 DoD    | Test         | TC-WCET-001 | Must     | Draft  | —         |
```

### Coverage row from the RTM's coverage section

```markdown
| Story | AC line                        | Requirements covering         |
|-------|--------------------------------|-------------------------------|
| 008   | Scenario L1 (heater de-assert) | FR-014                        |
| 008   | Scenario L2 (event raised)     | FR-015                        |
| 008   | Scenario L3 (latched state)    | FR-016                        |
| 008   | Rule "T_max from config"       | (covered by story 014 / FR-022) |
```

## What this example demonstrates

- **One AC line → one requirement.** The compound `Then … And … And …`
  in the original AC produced three separate functional requirements.
  Compound AC are a story-side construct; on the requirements side
  they split.
- **AC's `When` → EARS `When`, AC's `Given <state>` → EARS `While`.**
  FR-016 uses `While …, the controller shall …` because the AC
  describes sustained behavior in a state.
- **DoD-implied performance is captured separately.** The 5 ms WCET
  came from a story DoD item, not from a story AC. It became its own
  Performance requirement (PR-002) with its own parent reference.
- **Cross-references replace duplication.** Concern 4 ("T_max sourced
  from config") was already covered by story 014; the RTM coverage
  row points at the existing requirement instead of fabricating a
  duplicate.
- **Inferred thresholds are flagged.** The 5 ms WCET in PR-002 is
  derived from the 10 Hz sample period; the Notes line and the
  Assumptions section both record the inference.
- **Missing safety context is surfaced, not faked.** The "safe state"
  semantics could be a Safety requirement (SF-) with an integrity
  level if a hazard analysis were supplied. None was, so the behavior
  is captured as Functional and the gap is recorded as an Open
  Question. No invented integrity level.
- **Verification method is mandatory.** Every requirement names one of
  Inspection / Analysis / Demonstration / Test, populated in both the
  SRS body and the RTM.
