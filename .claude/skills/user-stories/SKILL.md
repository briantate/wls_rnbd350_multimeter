---
name: user-stories
description: Generate a backlog of agile user stories with acceptance criteria, definition of done, and definition of ready from a plain-text system or module description, and save to a Markdown file (or one file per story when requested). Use when the user provides a system description, module spec, requirements document, or feature brief and asks to extract user stories, build a backlog, or write stories with acceptance criteria. Trigger phrases include "extract user stories", "write user stories", "generate a backlog", "turn this spec into stories". Produces stories in Connextra form ("As a... I want... so that...") with Given-When-Then or checklist AC, classified by type (user-facing / enabler / technical) and layer, grouped under epics, with explicit source traceability. Handles embedded and firmware specs where user-facing stories must be decomposed into enabler stories so implementation cost is visible.
---

# User Stories

Turn a plain-text system or module description into a backlog of well-formed
user stories with acceptance criteria, classified by type and layer, with
parent–child decomposition for enabler work, plus a backlog-level
Definition of Done and Definition of Ready.

## Inputs

The input is one or more of:

- A short brief (one or two pages) describing a system or feature.
- A long specification (10–50+ pages) describing modules, behaviors, and
  constraints.
- A bulleted or numbered requirements list.
- A mixed document (prose plus tables, lists, or screenshots).

If the user has not specified an input file, ask which file or text to use
before drafting anything.

## Output

Default output file: `user-stories.md` in the current working directory,
unless the user specifies a different path.

If the user asks for one file per story (e.g., `001-...md`, `002-...md`
under `docs/stories/`), produce that layout, plus an index file
(`000-index.md`) that holds the summary, persona glossary, story map,
assumptions, open questions, out-of-scope list, **Definition of Done**,
and **Definition of Ready**.

The output file structure is in `assets/output-template.md`. Read it
before writing the result, and follow it. Do not invent additional
top-level sections without a reason.

## Story types and layers

Every story carries two classifications. They are required.

### Type

- **User-facing** — describes an outcome a human user (operator,
  technician, end customer) directly observes. Persona is a human role.
- **Enabler** — describes implementation work that supports a user-facing
  story. Persona is typically the firmware developer, the integrating
  application, or another component. These are real stories, not
  anti-patterns; without them, embedded estimation is wildly wrong.
- **Technical / NFR** — cross-cutting non-functional concerns
  (portability, memory budget, worst-case execution time, MISRA-C
  compliance, ISR-safety, power). May or may not have a single human
  beneficiary; persona is usually the developer or the system itself.

### Layer

- **Product** — what the end product does, observable to a customer.
- **System** — coordination across multiple components or boards.
- **Component** — one bounded module within the system (e.g., this
  Temperature Controller).
- **Module** — a sub-piece inside a component (driver, HAL shim,
  algorithm core).

A user-facing story almost always sits at Product or System layer. An
enabler story sits at Component or Module layer. A technical/NFR story
can sit at any layer.

### Why both classifications

A "small" user-facing story can hide a sprint of enabler work. "Operator
turns the system on" looks trivial but decomposes into power-up
sequencing, peripheral init order, default-state load, watchdog start,
splash UI, sensor warmup, fault path, etc. Tagging type and layer makes
the iceberg visible during planning.

## Workflow

Follow these steps in order. Do not stream stories as you read.

### 1. Read the source end-to-end

Read the full input before drafting any story. A first pass establishes
shape: actors, system boundary, what is in scope vs. out of scope.
Drafting mid-read produces stories biased toward whatever the document
front-loaded.

For long inputs (10+ pages), make two passes: first to map structure
(headings, tables, glossary), then to extract.

### 2. Detect the input's layer

Classify the input itself:

- **Product brief / user-facing description** — generates mostly
  user-facing stories. Enabler stories appear when a user-facing story
  has hidden implementation cost; surface them via decomposition (step 6).
- **Component / module / firmware spec** — generates mostly enabler and
  technical stories. There is usually no user-facing parent in the
  source; infer one or two plausible parent stories at Product layer
  and label them as inferred. The Temperature Controller spec is this
  shape.
- **Mixed** — both layers present. Tag each capability with the layer
  it belongs to.

The classification determines which sections of the output template are
load-bearing and where assumptions get labeled.

### 3. Extract actors and capabilities

Build four working lists before writing any "As a..." sentence:

1. **Actors / personas** — every user role named or implied. Be
   specific. Include both human roles (operator, technician, customer)
   and technical roles (firmware developer integrating the module, the
   calling component) when the input is at component/module layer.
2. **Capabilities** — what each actor needs to do or observe. One
   observable behavior per entry. Tag each capability with its layer.
3. **System-level outcomes** — these become epic headings.
4. **Constraints, business rules, NFRs** — these become acceptance
   criteria, technical stories, or Definition of Done items, not
   user-facing stories. Mark them separately.

### 4. Group capabilities into epics, draft in epic order

Do not draft in document order. Document order interleaves unrelated
features. Group by epic (the system-level outcome from step 3), then
draft within each epic.

### 5. Write the card and acceptance criteria together

For each story, draft the Connextra card and immediately write 1–3
acceptance criteria. Do not write all cards first and come back for AC.
Writing AC in the same pass:

- Catches untestable cards while intent is fresh.
- Forces splits early — if you cannot fit the behavior in 1–3 AC, the
  story is too big.
- Surfaces missing benefits — if you cannot say how to verify the "so
  that" was achieved, the benefit is probably tautological.

**Card format (Connextra):**

```
As a <specific persona>,
I want <single capability>,
so that <verifiable outcome>.
```

**Required metadata block per story** — render as a bullet list so the
fields don't collapse into one paragraph when previewed:

```
- **Type:** User-facing | Enabler | Technical
- **Layer:** Product | System | Component | Module
- **Parent:** <story id of parent user-facing story, or — if top-level>
- **Persona:** <specific persona>
- **Priority:** Must | Should | Could | Won't
- **Estimate:** <t-shirt size or rough days>
- **Depends on:** <story ids, or —>
- **Status:** Draft | Ready | In progress | Done
- **Source:** <section / page / bullet from the input>
```

**Acceptance criteria — pick the right format:**

- **Given–When–Then** for behaviors and workflows. Each scenario is
  atomic and self-contained. Describe behavior in observable terms, not
  implementation details (no DOM IDs, no function names, no table names).
- **Checklist** for validation rules, NFRs, and simple constraints.
- **Hybrid** is normal: GWT for headline behaviors, checklist for
  supporting rules.

**Each scenario must be presented in its own subheading and its own
fenced block** so the story is skim-readable. Do not stack multiple
`Scenario:` blocks inside one fence.

Aim for **1–3 AC per story**. Four or more is a signal to split.

### 6. Decomposition pass (embedded-aware)

Distinct from SPIDR splitting (which divides one story into smaller
peers). Decomposition is layered: a user-facing story spawns enabler
children that together implement it.

For each user-facing story, ask: **is the implementation cost obvious or
hidden?** If hidden, generate enabler stories with explicit parent links.

Decomposition prompts for embedded:

- Power and clock initialization sequence
- Peripheral initialization order and dependencies
- HAL boundary: what the module owns vs. what the HAL owns
- ISR vs. main-loop split; reentrancy and ISR-safety
- Default-state load on boot; recovery state on fault
- Watchdog start / kick / fault paths
- Persistence: what survives a reset, where it lives, when it's written
- Error propagation: from HAL → component → application → user-visible
- Timing: sample period enforcement, jitter budget, worst-case execution
- Test surface: how the component is exercised off-target

Each enabler child must still satisfy INVEST and trace to a real source
sentence (or be flagged in Assumptions).

### 7. INVEST review pass

Re-read each story and check each letter:

- **I**ndependent — can it be built without depending on another story
  in the same iteration? (Parent-child link does not violate
  Independent — the child is independently buildable; the parent is
  not "Done" until its children are.)
- **N**egotiable — does it describe an outcome, not an implementation?
- **V**aluable — does the benefit clause name a real outcome for the
  persona? For enabler stories, "value" is to the consuming developer
  or component, not to a human user.
- **E**stimable — is there enough context to size it?
- **S**mall — would a team finish it in a few days? If not, split.
- **T**estable — can the AC be verified by observing the system?

When a story fails S, split using SPIDR:

- **S**pike — extract a research story if unknowns dominate
- **P**aths — split by workflow path or branch
- **I**nterfaces — split by platform, browser, device, channel
- **D**ata — split by data subset (one type, region, or tenant first)
- **R**ules — relax a business rule for v1, add it back later

Every split must still deliver value (preserve V).

### 8. Definition of Done and Definition of Ready (backlog level)

Write both at the backlog level (in the index), not per story. AC asks
"is this behavior correct?" — DoD asks "is this work shippable?" — DoR
asks "is this story safe to pull into a sprint?"

A reasonable embedded DoD covers: unit tests on host with a test-double
HAL meeting a coverage target; on-target smoke test; replay/trace test
where applicable; static analysis clean; MISRA-C compliance clean (or
deviations recorded); worst-case execution time measured and within
budget; code review approved; public API documented; AC verified.

A reasonable DoR covers: AC reviewed and unambiguous; persona and
benefit confirmed; open questions resolved or labeled as low-risk
assumptions; story sized small enough to fit in a sprint; dependencies
identified; estimate agreed.

Tailor both lists to the project's domain. Safety-critical adds items
(e.g., requirements traceability, hazard analysis review). Web/SaaS adds
others (e.g., feature flag wired, telemetry confirmed).

### 9. Traceability check and source-coverage table

Every story must trace to a sentence, bullet, or section in the source.
If you cannot point to where a story came from, it is invented. Either:

1. Cut it, or
2. Move it to the **Assumptions** section with an explicit note.

Build a **source-coverage table** in the index: every paragraph or
numbered requirement in the source maps to at least one story id. Gaps
in the table are stories you missed; orphan stories with no row are
stories to cut or move to Assumptions.

## Quality bar

A good story:

- Names a **single capability** in the "I want" clause. No "and", "or",
  "also" — those signal compound stories.
- Has a **persona specific enough to drive design choices**. "User" is
  too generic. "First-time buyer with no account" is concrete. For
  enabler stories, "developer" is too generic; prefer "firmware
  developer integrating the controller into a product".
- Has a **benefit that names a verifiable outcome**, not a restatement
  of the capability. "...so that I can log in" after "I want to log in"
  is empty. "...so that I can resume work without contacting support"
  is real. For enabler stories, the benefit names what the consuming
  developer or component gains (predictability, testability,
  portability, observable failure modes).
- Has **acceptance criteria written in observable behavior**, not
  internals. A reader without codebase access can tell pass from fail.
- Covers **at least one boundary case** when the boundary is in scope
  (invalid input, missing precondition, error path).
- **Fits on a card.** If the card needs its own section header, it is
  an epic in disguise.
- Is **classified** with type, layer, and parent (or `—`).

## Refusal list — anti-patterns to reject

Refuse to produce these. If the source pushes you toward one, surface
the issue and propose a fix.

- **Vague stories** — "I want a better dashboard so it is more useful."
  Rewrite with a specific capability and outcome, or flag as
  insufficient input.
- **Compound stories** — "I want X and Y." Split into separate stories.
- **Untestable AC** — "fast", "intuitive", "user-friendly", "secure"
  with no measurable threshold. Either propose a threshold and flag it
  as an assumption, or refuse.
- **Missing benefit** — no "so that" clause. Do not silently fabricate
  one. Extract it from context if possible; otherwise flag the gap.
- **Implementation-coupled AC** — "When the user clicks `#submit-btn`...".
  Rewrite as observable behavior ("When the user submits credentials...").
- **Invented stories** — features not traceable to the source. Move to
  Assumptions or cut.
- **Fake human desires** — "As a user, I want to enter a strong
  password." Real users don't want that; the security stakeholder
  does. Re-cast with the real stakeholder ("As a security
  officer...") or reframe as the underlying user goal ("...so that my
  account is safe"). Note: this rule applies only to fake *human*
  desires. Enabler stories with technical personas ("As the
  application layer, I want a deterministic update API…") are
  legitimate when the input is at component/module layer; classify
  them as Enabler, not user-facing.
- **Layer confusion** — pretending an enabler story is user-facing by
  stuffing technical content into a human-persona card ("As a user, I
  want a 100 ms sample period"). The user does not want that. Re-cast
  with a technical persona and tag Type as Enabler.
- **Hidden iceberg** — accepting a one-line user-facing story without
  decomposing it when implementation cost is non-trivial. "Operator
  turns the system on" is the canonical example. Run the decomposition
  pass (step 6) and produce enabler children before declaring the
  backlog complete.
- **Prescriptive UI in the "I want"** — pixel and click choreography
  belong in design specs. The card describes intent, not interaction
  scripts.

## Ask vs. push through

**Stop and ask** when:

- The persona is genuinely unclear and the choice changes the design
  (e.g., "operator" could mean technician or scheduler).
- Safety, regulatory, or security context is missing in a domain that
  obviously requires it (medical, automotive, financial, embedded
  control). Do not guess at a safety-critical AC.
- Two parts of the source contradict each other on a load-bearing
  behavior (not a typo — an actual conflict).
- The source explicitly says "TBD" or leaves a placeholder.
- A capability has no plausible benefit and you cannot infer one.
- Scope is ambiguous in a way that changes the story count by an
  order of magnitude.

**Push through with a labeled assumption** when:

- The persona is unstated but strongly implied. Name the assumption.
- A benefit is unstated but directly derivable from the surrounding
  paragraph or system purpose.
- A boundary case is standard practice in the domain (reject empty
  input, require auth on state-changing endpoints). Add the AC and
  tag it as a standard-practice assumption.
- The input is a module spec with no user-facing parent in the source.
  Infer one or two plausible parent user-facing stories at Product
  layer and label them as inferred.
- Terminology is inconsistent in trivial ways (singular/plural,
  casing). Normalize and note it.
- The source has structural noise (sales copy, history,
  acknowledgements). Drop it.

Rule of thumb: **ask when guessing wrong would produce stories that
look right but aren't**; push through with a labeled assumption when
the gap is small enough that a reviewer can correct it in one pass.

## Voice and conventions

- Use the source's terminology. If the source says "tenant", do not
  rewrite as "customer".
- Use imperative, present-tense verbs in the "I want" clause:
  "submit", "view", "configure" — not "be able to submit".
- Number stories sequentially across the whole backlog (`001`, `002`,
  ...), not per-epic. Cross-references in the metadata block carry the
  epic and parent links.
- Keep each story self-contained — a reader should not need to load
  another story to understand this one.

## Worked examples

For input/output examples that demonstrate the bar, read
[references/examples.md](references/examples.md). It contains:

- **Example 1** — a short brief (interpretation-heavy single-layer
  case). Shows handling of implied personas and benefits.
- **Example 2** — a long mixed spec (triage-heavy). Shows layer
  tagging, persona splitting, and conflict surfacing as open questions.
- **Example 3** — an embedded user-facing-to-enabler decomposition.
  Shows how a single user-facing story spawns multiple enabler
  children with parent links, so the implementation iceberg is
  visible.

Read Example 3 whenever the input is at component or module layer, or
whenever the input is a user-facing brief whose stories are likely to
hide non-trivial implementation cost.

## Output template

The exact output file structure is in
[assets/output-template.md](assets/output-template.md). Read it before
writing the result. Copy the structure; fill in the content. Do not
invent additional top-level sections without a reason.
