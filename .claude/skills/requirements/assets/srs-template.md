# Software Requirements Specification — <System Name>

**Source backlog:** `<path/to/docs/stories/ or input file>`
**Generated:** <YYYY-MM-DD>
**Conforms to:** ISO/IEC/IEEE 29148:2018 structure (tailored)

---

## 1. Introduction

### 1.1 Purpose

<One paragraph. What this SRS specifies and who it is for.>

### 1.2 Scope

<What the system *is*, what it *does*, and what is explicitly out of
scope. Reference the source backlog's Out-of-scope section.>

### 1.3 Definitions, acronyms, abbreviations

A glossary of every domain term used in requirements. Use the source
backlog's persona glossary and any terminology it introduces. One line
per entry.

- **<Term>** — <definition>.
- ...

### 1.4 References

- Source backlog: `<path>`
- Inherited standards (if any): ISO 26262 / DO-178C / IEC 62304 /
  IEC 61508 / MISRA-C:2012 / OWASP / etc.
- External interface specifications referenced by IR-* requirements.

### 1.5 Overview

<Brief reading guide for the document. Pointer to §3 for the
requirements catalog and to the RTM (`rtm.md`) for traceability.>

---

## 2. Overall description

### 2.1 Product perspective

<Where the system sits relative to its environment. For embedded
modules: physical placement, host application, hardware platform,
HAL boundary.>

### 2.2 Product functions

<Short bulleted list of high-level capabilities. Names only — details
in §3.>

### 2.3 User characteristics

<Personas from the source backlog, abridged.>

### 2.4 Constraints, assumptions, dependencies

<Carry forward the source backlog's Assumptions section. List inherited
standards and the obligations they impose.>

### 2.5 States and modes overview

<If the system has a non-trivial state model, summarize it here with a
state diagram or state table. Detailed transition requirements live in
§3.10.>

---

## 3. Specific requirements

Each subsection below contains the requirements of that type. Every
requirement is in EARS form. Every requirement carries a metadata
block: Type, Verification, Priority, Parent. Every requirement has a
unique ID with the type's prefix.

### 3.1 Functional requirements (FR-*)

#### FR-001. <Short title>

**Statement.** <EARS-form requirement.>

- **Type:** Functional
- **Verification:** Test
- **Priority:** Must | Should | Could
- **Parent:** <Story id / AC line / standard clause>
- **Notes:** <optional — only when something is non-obvious>

#### FR-002. <Short title>

<...>

### 3.2 Performance requirements (PR-*)

#### PR-001. <Short title>

**Statement.** <EARS-form requirement with quantified threshold.>

- **Type:** Performance
- **Verification:** Test (instrumented) | Analysis
- **Priority:** Must
- **Parent:** <reference>

### 3.3 External interface requirements (IR-*)

For each external interface, name the interface partner, the protocol,
the message format, and any byte-/bit-ordering or unit conventions.

#### IR-001. <Short title>

**Statement.** <EARS-form requirement.>

- **Type:** Interface
- **Verification:** Inspection | Test
- **Priority:** Must
- **Parent:** <reference>

### 3.4 Environmental & physical requirements (ER-*)

<Operating temperature, vibration, humidity, EMI/EMC, mounting, mass,
dimensions, ingress protection.>

### 3.5 Resource requirements (RR-*)

<Memory (flash, RAM, stack), CPU%, power, bandwidth, storage budgets.>

### 3.6 Reliability requirements (REL-*)

<Uptime, MTBF, fault tolerance, graceful degradation.>

### 3.7 Security requirements (SEC-*)

<Authentication, authorization, integrity, confidentiality, audit,
crypto obligations. Map to threat model where one exists.>

### 3.8 Usability requirements (UR-*)

<Human-factors targets. Quantified — task completion time, error rate,
accessibility conformance level.>

### 3.9 Safety requirements (SF-*)

<Hazardous-state avoidance and fail-safe obligations. Each carries an
integrity level (ASIL / DAL / SIL) when the domain mandates one. Trace
to hazard analysis as well as to the originating story.>

#### SF-001. <Short title>

**Statement.** <EARS-form requirement.>

- **Type:** Safety
- **Integrity level:** ASIL B | DAL C | SIL 2 | —
- **Verification:** Test + Analysis
- **Priority:** Must
- **Parent:** Story <id>; <hazard analysis ref or standard clause>

### 3.10 States and modes requirements (SM-*)

<Mode definitions and transition behaviors.>

### 3.11 Design constraints (DC-*)

<Externally imposed constraints: regulations, standards, customer
mandates, coding standards (e.g., MISRA-C). Each traces to the
inherited document, not to a free-floating preference.>

---

## 4. Verification

Every requirement carries a verification method (Inspection, Analysis,
Demonstration, Test) in its metadata block. This section defines the
methods themselves, references the test plan, and lists any
verification activities not bound to a single requirement (e.g.,
integration tests, acceptance demonstrations).

| Method | Definition |
|---|---|
| **Inspection** | Visual examination — labeling, documentation, source code. |
| **Analysis** | Calculation or modeling without running the system — WCET proofs, memory budget calc, FMEA. |
| **Demonstration** | Operate the system and observe — boot sequence, UI flow. |
| **Test** | Formal pass/fail against quantified criteria, usually automated. |

---

## 5. Traceability

The full bidirectional Requirements Traceability Matrix is in
[`rtm.md`](rtm.md). It links every requirement to its parent (story id,
AC line, or standard clause) and to its forward verification (test
case id) when known.

---

## 6. Open questions

Items requiring clarification before the affected requirements can be
finalized. Each entry should reference the requirement(s) it blocks.

- **<question>** — blocks **<ReqID>**. <Brief context.>
- ...

## 7. Assumptions

Requirements or thresholds inferred from context rather than stated
directly in the source. Each entry names what was assumed and the
basis for the inference.

- **<ReqID — assumption>** — <what was assumed and why>.
- ...

## 8. Out of scope

Items present in the source backlog but explicitly excluded from this
SRS.

- <item> — <reason / source reference>
- ...

## Appendix A. Glossary

<Full glossary of domain terms — superset of §1.3 if needed.>

## Appendix B. Change log

| Date | Change | Author |
|---|---|---|
| <YYYY-MM-DD> | Initial draft | <name> |
