# Requirements Traceability Matrix — <System Name>

**Companion to:** [`srs.md`](srs.md)
**Generated:** <YYYY-MM-DD>

A bidirectional traceability matrix. Every requirement traces
**backward** to its parent (story id, AC line, hazard analysis, or
standard clause) and **forward** to its verification (test case id)
when known.

---

## 1. Requirements → parent and verification (forward / backward)

Sort by ReqID. One row per requirement. The columns are required;
omit a value only when truly unknown (mark with `—`) and surface the
gap in §4.

| ReqID  | Type | Statement (summary)                       | Parent (story / clause)             | Verification | Test case   | Priority | Status | Integrity |
|--------|------|-------------------------------------------|-------------------------------------|--------------|-------------|----------|--------|-----------|
| FR-001 | F    | <one-line summary of FR-001>              | Story 003 / AC scenario L1          | Test         | TC-INIT-001 | Must     | Draft  | —         |
| FR-002 | F    | <...>                                     | Story 004 / AC scenario L1          | Test         | TC-UPD-001  | Must     | Draft  | —         |
| PR-001 | P    | <...>                                     | Story 004 DoD WCET item             | Test         | TC-WCET-001 | Must     | Draft  | —         |
| IR-001 | I    | <...>                                     | Story 004 / AC scenario             | Inspection   | TC-API-001  | Must     | Draft  | —         |
| SF-001 | S    | <...>                                     | Story 008; ISO 26262 hazard H-3     | Test+Anal.   | TC-OT-001   | Must     | Draft  | ASIL B    |
| DC-001 | C    | MISRA-C:2012 mandatory rules clean        | Project quality plan §4.2           | Inspection   | —           | Must     | Draft  | —         |
| ...    | ...  | ...                                       | ...                                 | ...          | ...         | ...      | ...    | ...       |

Type letter codes:
F = Functional, P = Performance, I = Interface, E = Environmental,
R = Resource, REL = Reliability, SEC = Security, U = Usability,
S = Safety, M = States/Modes, C = Design Constraint.

---

## 2. Stories → requirements (coverage from the story side)

For every story (and every AC line within it), list the requirements
that cover it. Gaps in this section are **missed requirements**.

| Story | AC line / DoD item               | Requirements covering    | Notes |
|-------|----------------------------------|--------------------------|-------|
| 001   | Scenario L1 — holds setpoint     | FR-014, FR-015           | Epic-level; coverage via children |
| 003   | Scenario L1 — fresh init         | FR-001, FR-002           |       |
| 003   | Scenario L2 — invalid config     | FR-003                   |       |
| 004   | Scenario L1 — periodic update    | FR-004                   |       |
| 004   | Rule "no HW reads"               | IR-001 (covered)         | Verifiable by inspection |
| 004   | DoD WCET item                    | PR-001                   | Threshold 5 ms (assumption) |
| 008   | Scenario L1 — heater de-assert   | FR-014, SF-001           |       |
| 008   | Scenario L2 — event raised       | FR-015                   |       |
| 008   | Scenario L3 — latched state      | FR-016                   |       |
| ...   | ...                              | ...                      | ...   |

Coverage summary:

- **Total stories:** N
- **Total AC lines / DoD items:** M
- **Covered:** X (%)
- **Uncovered:** Y — listed below.

Uncovered items (each is either a missed requirement or an explicitly
deferred item):

- <story id / AC line> — <reason: deferred to v2 / blocked by open
  question / out of scope>.
- ...

---

## 3. Standards → requirements (when applicable)

For each inherited standard, list the clauses that bind into this SRS
and which requirements address them. Required for safety-critical /
regulated work.

| Standard | Clause | Topic | Requirements covering |
|---|---|---|---|
| ISO 26262 | hazard H-3 | Over-temperature thermal runaway | SF-001 |
| MISRA-C:2012 | All mandatory + required rules | Coding standard | DC-001 |
| ... | ... | ... | ... |

---

## 4. Open trace gaps

Items where the trace is known to be incomplete. Each gap has an
owner and a target resolution.

- **<ReqID or story id>** — <description of gap>. Owner: <name>.
  Target: <date or milestone>.
- ...

---

## 5. Living-document note

This matrix is regenerated whenever requirements or stories change.
Stale rows (a requirement that no longer exists; a story whose AC has
been edited) are detected by re-running the requirements skill against
the current story backlog and diffing against this file.
