# User Stories — <System or Module Name>

**Source:** `<path/to/input file or short description of input>`
**Generated:** <YYYY-MM-DD>
**Input layer:** <Product brief | Component / module spec | Mixed>

## Summary

<2–4 sentences. What the system or module does, who uses it, and the
overall scope of this backlog. No marketing language.>

## Personas

A glossary of every actor referenced in the stories. One line each.
Distinguish human personas (operators, technicians, end customers) from
technical personas (firmware developer integrating the module, calling
component) when the backlog mixes layers.

- **<Persona name>** — <short description: their role, what they care
  about, and any defining context>.
- ...

## Story map

A tree showing user-facing parents with their enabler children indented
underneath. Use this to spot icebergs (a tiny user-facing parent with a
long list of children).

- **001 — <user-facing parent title>** _[User-facing, Product]_
  - 003 — <enabler child title> _[Enabler, Component]_
  - 004 — <enabler child title> _[Enabler, Component]_
- **002 — <user-facing parent title>** _[User-facing, Product]_
  - 011 — <enabler child title> _[Enabler, Component]_
- _Top-level enablers and technical/NFR stories (no parent):_
  - 015 — <story title> _[Technical, Module]_
  - ...

## Definition of Ready

A story is Ready to be pulled into a sprint when:

- [ ] Acceptance criteria are reviewed and unambiguous.
- [ ] Persona and benefit are confirmed (not inferred placeholders).
- [ ] Open questions affecting this story are resolved or labeled as
      low-risk assumptions.
- [ ] Story is small enough to fit in a sprint.
- [ ] Dependencies are identified and the dependency story is itself
      Ready or Done.
- [ ] Estimate is agreed by the team.
- [ ] _<add domain-specific items here>_

## Definition of Done

A story is Done when **all** of the following are true. This list is
backlog-wide; per-story AC verifies behavior, DoD verifies shippability.

- [ ] Acceptance criteria pass on host with a test-double HAL/dependency.
- [ ] Coverage target met on the changed code (e.g., ≥ 90% line, ≥ 80%
      branch).
- [ ] On-target smoke test passes on the reference hardware.
- [ ] Replay or trace test passes for the changed code path (if
      applicable to the story).
- [ ] Static analysis is clean (or new findings are recorded as
      deviations).
- [ ] MISRA-C compliance is clean (or new deviations are recorded with
      justification). _(Embedded only — drop if not applicable.)_
- [ ] Worst-case execution time and memory budget are measured and
      within target. _(Embedded / real-time only.)_
- [ ] Public API is documented (e.g., Doxygen) where the story changed
      a public interface.
- [ ] Code review approved by at least one reviewer.
- [ ] Merged to the integration branch with CI green.
- [ ] _<add domain-specific items here>_

## Source coverage

Every paragraph or numbered requirement of the source must map to at
least one story. Gaps in this table are stories you missed.

| Source location | Topic | Story ids |
|---|---|---|
| ¶1 | <topic> | 001, 003 |
| ¶2 | <topic> | 004, 014 |
| ... | ... | ... |

## Backlog

### Epic E1 — <Epic title (a system-level outcome)>

<One sentence describing the outcome this epic delivers. Optional
**Source:** line pointing to the section/page that motivates the epic.>

#### 001 — <Short story title>

> As a <persona>,
> I want <single capability>,
> so that <verifiable outcome>.

- **Type:** User-facing | Enabler | Technical
- **Layer:** Product | System | Component | Module
- **Parent:** <story id of parent user-facing story, or — if top-level>
- **Persona:** <specific persona>
- **Priority:** Must | Should | Could | Won't
- **Estimate:** <t-shirt size or rough days>
- **Depends on:** <story ids, or —>
- **Status:** Draft | Ready | In progress | Done
- **Source:** <section / page / bullet from the input>

##### Acceptance criteria

###### Scenario: <name of the headline behavior>

```gherkin
Given <preconditions>
When  <action>
Then  <observable outcome>
```

###### Scenario: <name of the next behavior>

```gherkin
Given <preconditions>
When  <action>
Then  <observable outcome>
```

###### Additional rules (if any)

- <Checklist rule 1>
- <Checklist rule 2>

**Notes:** <optional — only when something is non-obvious; otherwise omit>

#### 002 — <Short story title>

<...same structure...>

---

### Epic E2 — <Epic title>

<...same structure...>

## Assumptions

Stories or AC not directly traceable to the source but inferred. Each
entry names what was assumed and why. A reviewer can use this list to
confirm or correct each item.

- **<001 — assumption>** — <what was assumed, and the basis for the
  inference (domain standard, surrounding context, common practice)>.
- ...

## Open questions

Items requiring clarification before the affected stories can be
finalized. Each entry should reference the story it blocks.

- **<question>** — blocks **<002>**. <Brief context: what is unclear
  and what answer would unblock it.>
- ...

## Out of scope

Items present in the source but explicitly excluded from this backlog
(marked "future", "v2", "out of scope", or otherwise deferred).

- <item> — <reason / source reference>
- ...

---

## One-file-per-story layout (when requested)

When the user asks for one file per story (e.g., `docs/stories/001-...md`),
produce:

- `000-index.md` — contains everything above _except_ the per-story
  cards in the **Backlog** section. The index keeps Summary, Personas,
  **Story map**, **Definition of Ready**, **Definition of Done**,
  **Source coverage**, Assumptions, Open questions, and Out of scope.
  Replace the inline backlog with a list of story links grouped by
  epic.
- One file per story, named `NNN-short-slug.md`. Each story file
  contains: title heading, Connextra card (blockquote), full metadata
  block, acceptance criteria with each scenario in its own subheading
  and its own fenced block, and Notes if any. Do not duplicate the
  backlog-level DoD/DoR/persona glossary inside the story file —
  reference `000-index.md` instead.
