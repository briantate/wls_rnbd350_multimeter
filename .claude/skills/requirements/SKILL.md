---
name: requirements
description: Decompose a set of user stories into traceable system-level requirements using EARS notation, classified by ISO/IEC/IEEE 29148 type (functional, performance, interface, environmental, resource, reliability, security, usability, safety, states-and-modes, design constraints), and produce a 29148-aligned Software Requirements Specification with a bidirectional Requirements Traceability Matrix. Use when the user provides a story backlog (typically the output of the user-stories skill in docs/stories/) and asks to extract requirements, write an SRS, build a requirements spec, decompose stories into requirements, or build a traceability matrix. Trigger phrases include "extract requirements", "write requirements", "generate an SRS", "decompose stories into requirements", "build a traceability matrix", "RTM". Pairs with the user-stories skill — its primary input is the story files produced by user-stories (000-index.md plus per-story 00N-…md files with Type/Layer/Parent metadata and GWT/checklist AC).
---

# Requirements

Decompose a set of user stories into a 29148-aligned Software Requirements
Specification (SRS) with EARS-formatted requirements organized by type, plus
a bidirectional Requirements Traceability Matrix (RTM) that links every
requirement back to its parent story and forward to its verification.

## Inputs

The primary input is a story backlog produced by the `user-stories` skill:

- `docs/stories/000-index.md` — summary, persona glossary, story map,
  DoR/DoD, source-coverage table, assumptions, open questions.
- `docs/stories/0NN-...md` — per-story files with a Connextra card,
  metadata block (Type / Layer / Parent / Persona / Priority / Estimate /
  Depends on / Status / Source), and acceptance criteria in
  Given–When–Then or checklist form.

Also accept:

- A flat `user-stories.md` document with stories in one file.
- A loose bullet list of stories. Lower-quality input — see "Input
  variability" below.
- A wiki-prose mix. Triage first; treat background as context, not
  requirements.

If the user has not specified an input, ask which file or directory to
use before drafting anything.

## Output

Default output file: `docs/requirements/srs.md`, plus
`docs/requirements/rtm.md` for the traceability matrix.

The exact output structure is in `assets/srs-template.md` (SRS) and
`assets/rtm-template.md` (RTM). Read both before writing the result;
copy the structure; fill in the content.

## Workflow

Follow these steps in order. Do not stream requirements as you read.

### 1. Read the entire story backlog before drafting

Read `000-index.md` first to understand the backlog's shape — story map,
DoR/DoD, source coverage, assumptions, open questions, out-of-scope.
Then read every per-story file. For inputs that aren't from the
user-stories skill, read the input end-to-end and extract the same
metadata implicitly.

### 2. Triage open questions and contradictions

Open questions in the input that affect a load-bearing behavior
(safety, integrity level, interface partner, threshold) **block**
requirements in that area. Do not write requirements over an
unresolved load-bearing open question — list them explicitly in the
Open Questions section of the SRS and skip the affected requirements.

If two stories contradict each other on a load-bearing point, refuse to
write requirements in the conflict zone until reconciled. Surface the
conflict in Open Questions.

### 3. Extract concerns from each story

A "concern" is a candidate requirement: one observable behavior, one
constraint, one interface obligation, or one quality target. For each
story:

1. Read each AC line. Each is a candidate concern.
2. Read the "I want" / "so that" clauses. They occasionally yield
   concerns the AC missed.
3. Read the story metadata. Layer, Type, and DoD-implied properties
   sometimes yield non-functional concerns.

Concerns at this stage are unclassified and unwritten. They are notes.

### 4. Classify each concern by type

Tag every concern with one of: **Functional**, **Performance**,
**Interface**, **Environmental**, **Resource**, **Reliability**,
**Security**, **Usability**, **Safety**, **States and modes**, or
**Design constraint**.

The classification decision rules and worked examples are in
[references/types.md](references/types.md). Read it whenever you are
unsure where a concern goes — this is the single most common place
requirements skills go wrong.

### 5. Decide decomposition strategy: per-story or theme-clustered

- **Functional concerns** decompose **per story** by default. Each
  story produces a small group of functional requirements traced 1:1 or
  1-to-many back to the story.
- **NFRs that cut across many stories** (e.g., "every operator-facing
  flow must be usable in under 3 s") are **clustered by theme**, with
  the parent trace pointing to the set of affected stories or to
  "product-level cross-cutting".
- **Safety / regulatory concerns** decompose **per clause** of the
  governing standard, with parent traces back to both the standard
  clause and the stories that exercise the affected behavior.

### 6. Write requirements in EARS notation

For each classified concern, write a single EARS-form requirement.
The five EARS patterns and selection rules are in
[references/ears.md](references/ears.md). Read it whenever you are
deciding which pattern fits.

Quick guidance:

- Map AC `When … Then …` → EARS event-driven (`When …, the <system>
  shall …`).
- Map AC `Given <state> … Then …` → EARS state-driven (`While …, the
  <system> shall …`).
- Map AC checklist rules and ubiquitous invariants → EARS ubiquitous
  (`The <system> shall …`).
- Map negative-path scenarios and unwanted conditions → EARS unwanted
  behavior (`If …, then the <system> shall …`).
- Map optional features → EARS optional (`Where …, the <system>
  shall …`).

Assign each requirement a unique ID with a type prefix (`FR-001`,
`PR-002`, `IR-003`, `SF-004`, etc.). The full prefix table is in
`references/types.md`.

### 7. Quantify every NFR

Replace any qualitative word from the source stories with a number and
unit. If the source did not supply a threshold, pick a domain-standard
default and **flag it as an assumption**. Untestable NFRs are defects.

### 8. Assign a verification method to every requirement

Pick one of: **Inspection**, **Analysis**, **Demonstration**, **Test**.
A requirement with no verification method is a defect — either rewrite
it so a method applies, or cut it.

### 9. Build the bidirectional traceability matrix as you go

Every requirement gets a row in the RTM with: ReqID, type, statement
(or summary), parent story id(s), parent standard clause(s), verification
method, planned test case id (if known), priority, status, integrity
level (if applicable). Do not write all requirements first and build
the RTM afterward — the matrix is the audit trail and grows in
lockstep.

### 10. Coverage and orphan check

Two passes at the end:

- **Forward coverage** — every story (and every AC line within it)
  maps to at least one requirement. Gaps are missed requirements.
- **Backward coverage** — every requirement has a non-empty parent
  reference. Orphans are either invented requirements (cut, or move
  to Assumptions with explicit justification) or true cross-cutting
  concerns (declare so explicitly with parent = "product-level").

Add a coverage summary table at the end of the RTM showing both
directions.

## Quality bar

A good requirement:

- States **one concern**. No "and", "or", "as well as", semicolons
  joining behaviors. Two verifications → two requirements.
- Uses **active voice with the system named as subject**. "The
  controller shall…" — not "Temperature shall be regulated…".
- Uses **"shall" as the only binding modal verb**. No "should /
  must / may" mixed in for binding requirements.
- Has a **quantified threshold** for every NFR. Numbers and units.
- Carries a **verification method**.
- Traces to a **parent** (story id, AC line, standard clause, or
  explicit "product-level cross-cutting").
- Names something **observable at the system boundary** (input,
  output, state, event), not an internal implementation detail.
- Uses **consistent vocabulary**, matching the input's glossary.
- Sits at a **layer appropriate to its parent**.
- Uses **positive phrasing** where possible. "Shall reject invalid
  inputs" beats "shall not accept invalid inputs".

## Refusal list — anti-patterns to reject

Refuse to produce these. If the input pushes you toward one, surface
the issue and propose a fix.

- **Untraced requirements** — every requirement must have a parent
  reference. Untraced → move to Assumptions or cut.
- **Invented requirements** — features not derivable from the source
  stories or a binding standard. Even "obviously needed" things
  (logging, auth, error handling) get flagged as assumptions if no
  story or standard supplies them.
- **Compound requirements** — split them.
- **Restated stories** — a requirement that just paraphrases the
  story without adding precision (a quantified threshold, an
  interface specification, a verification target, a state condition)
  is dead weight.
- **Untestable requirements** — no assignable verification method
  → reject. "Shall be intuitive / modern / robust" with no
  measurable property are rejected on sight.
- **Implementation prescription** — "shall use Postgres",
  "shall use AES-256" → reject unless mandated by an inherited
  standard (then trace to the standard, not to a free-floating
  preference).
- **Modal mush** — never mix "shall / should / may / must / will"
  for binding requirements. "Shall" only.
- **Negative-only requirements** when a positive form exists.
- **Pronoun-laden requirements** ("It shall…", "they shall…").
  Name the subject every time.
- **Speculative / future requirements** ("shall be extensible to
  support…") unless extensibility itself is measurable.
- **Slash ambiguity** — no "and/or"; replace with explicit
  logic.
- **Ambiguous quantifiers** — "all", "every", "any" without a
  defined set.

## Ask vs. push through

**Stop and ask** when:

- Safety, regulatory, or security context is missing in a domain
  that obviously requires it (medical, automotive, aerospace,
  industrial control, financial). Do not invent safety
  requirements; the hazard analysis is upstream.
- Two stories contradict each other on a load-bearing behavior.
- An NFR has no quantified threshold and no obvious domain
  default.
- The interface partner is unspecified ("shall communicate with
  the cloud" — which cloud, which protocol?).
- The integrity level is not assigned in a safety-critical context
  (ASIL / DAL / SIL).
- Scope ambiguity changes the requirement count by an order of
  magnitude.
- A story explicitly defers something to "TBD" or "v2".

**Push through with a labeled assumption** when:

- A standard-domain default exists (HTTP timeout 30 s, log
  retention 90 d, OWASP password rules, big-endian network byte
  order). Apply and tag.
- An interface boundary is implied but not stated (e.g., the
  controller spec names a BME280; treat the sensor interface as
  BME280-over-I2C and document it).
- An NFR threshold is derivable from an adjacent story (sample
  period implies WCET budget; flash size implies code-size budget).
- A verification method is unstated but obvious (a quantified
  performance requirement is verified by test; a labeling
  requirement is verified by inspection).
- A modal verb is missing in a requirement-shaped input
  ("the system records…"). Normalize to "shall" and note.
- The source uses inconsistent terminology trivially. Normalize
  against the glossary and note.

Rule of thumb: **ask when guessing wrong would create a contractual
obligation the team cannot meet** (safety thresholds, interface
partners, integrity levels); **push through with a labeled
assumption when the gap is small enough that a reviewer can
correct it in one editing pass**.

## Voice and conventions

- Use **"shall"** as the only binding modal verb.
- Use **"the <system>"** or the named subsystem as the subject of
  every requirement. Define `<system>` in the SRS introduction.
- Use the **glossary terminology** from the story backlog. If the
  backlog says "controller", every requirement says "controller".
- Use **logical-expression brackets** for compound conditions:
  `[X AND Y]`, `[X OR Y]`, `NOT [X]`. Avoid English ambiguity.
- **Number requirements with a type prefix and a stable id** —
  e.g., `FR-001`, `PR-001`, `IR-001`, `SF-001`. Stable ids matter
  because the RTM, design docs, and tests reference them.
- Keep each requirement **self-contained** — a reader should not
  need to load another requirement to understand this one.

## Worked examples

For a worked input/output example, read
[references/examples.md](references/examples.md). Read it whenever:

- The input is a small backlog and you want to see the minimum
  shape of a complete SRS + RTM.
- You need a reference for how AC scenarios map to EARS patterns
  and how the RTM is populated.

## Output template

The exact output structure is in:

- [assets/srs-template.md](assets/srs-template.md) — the SRS document
  layout (introduction, overall description, specific requirements
  by type, verification, traceability, appendices).
- [assets/rtm-template.md](assets/rtm-template.md) — the bidirectional
  traceability matrix layout.

Read both before writing the result. Do not invent additional
top-level sections without a reason.
