# EARS Notation Reference

The Easy Approach to Requirements Syntax (Mavin et al.) — a small,
fixed set of patterns that produce unambiguous requirements without a
formal language. Use EARS for every requirement statement in the SRS.

## The five patterns

### 1. Ubiquitous

Always-active invariants. No keyword.

```
The <system> shall <response>.
```

**Use for:** invariant properties, ubiquitous obligations, things that
hold for the system's entire lifetime.

**Example:**
> The temperature controller shall report its current control state
> through the public query interface.

### 2. State-driven

Active while a state condition holds. Keyword: **While**.

```
While <state>, the <system> shall <response>.
```

**Use for:** behaviors gated by a sustained system state. Maps cleanly
from `Given <state>` clauses in GWT acceptance criteria.

**Example:**
> While the controller is in over-temperature safe state, the
> temperature controller shall hold the heater output de-asserted.

### 3. Event-driven

Triggered by a discrete event. Keyword: **When**.

```
When <trigger>, the <system> shall <response>.
```

**Use for:** stimulus-response behaviors. The most common form for
functional requirements derived from GWT scenarios — `When` maps to
the AC's `When`, `shall` to the `Then`.

**Example:**
> When an update is invoked with a temperature reading at or above the
> over-temperature threshold, the temperature controller shall
> de-assert the heater output within one update cycle.

### 4. Optional feature

Active only if a feature is present in the build/configuration.
Keyword: **Where**.

```
Where <feature is included>, the <system> shall <response>.
```

**Use for:** behaviors of optional or configurable features. Maps to
build-time flags, license-gated features, hardware variants.

**Example:**
> Where the BME280 humidity option is included, the temperature
> controller shall report relative humidity through the public query
> interface.

### 5. Unwanted behavior

Response to a fault, error, or unwanted condition. Keyword: **If/then**.

```
If <unwanted condition>, then the <system> shall <response>.
```

**Use for:** error paths, fault handling, defensive behavior. Maps from
GWT scenarios that exercise negative paths.

**Example:**
> If the configuration table contains a setpoint outside the configured
> valid range, then the temperature controller shall fail
> initialization and remain in a state where the heater output is
> de-asserted.

## Complex (combined) requirements

Patterns can be combined. The combination order is fixed: `Where` (if
present), then `While` (if present), then `When` or `If` (one of), then
the response clause.

**Example:**
> While the aircraft is on ground, when reverse thrust is commanded,
> the engine control system shall enable reverse thrust.

> Where the diagnostic interface is enabled, when the technician
> requests a fault history, the controller shall return the last
> 100 fault events.

Avoid combining more than two conditions in one requirement. If a
behavior has three or more guards, consider whether it is two
requirements.

## Pattern selection — quick decision tree

1. Is the behavior triggered by a discrete event or input? → **When**
   (event-driven).
2. Is the behavior active only while a state holds? → **While**
   (state-driven).
3. Is the behavior a response to an unwanted condition or fault? →
   **If/then** (unwanted behavior).
4. Is the behavior present only when a feature is configured/built in?
   → **Where** (optional).
5. Is the behavior an invariant that always holds? → **Ubiquitous** (no
   keyword).

If two patterns seem to fit, prefer the more specific (event-driven over
ubiquitous; state-driven over ubiquitous; unwanted-behavior over
event-driven for error paths).

## Mapping AC formats to EARS patterns

The user-stories skill produces two AC formats. Map them like this:

### Given–When–Then scenarios

```gherkin
Given <state>
When  <trigger>
Then  <observable outcome>
```

Maps to combined state-driven + event-driven:

> While `<state>`, when `<trigger>`, the `<system>` shall `<observable
> outcome>`.

If `Given` is empty or trivially "an initialized system", drop the
`While` clause and use plain event-driven.

If the scenario describes an unwanted condition (Given describes a
fault, When describes input that exercises the fault path), prefer
**If/then** over **When**.

### Checklist rules

Single bullet rules in AC translate to:

- **Ubiquitous** for invariants ("The reset link expires 24 hours
  after issue" → `When the issuance time of a reset link reaches 24
  hours, the system shall mark the link as expired.` or, equivalently,
  `The system shall expire reset links 24 hours after issuance.`).
- **If/then** when the rule is a constraint on an unwanted condition
  ("An expired link is rejected" → `If a link past its expiry is
  presented, then the system shall reject the request and respond with
  an "expired" error.`).

## Anti-patterns to avoid

- **Mixing modal verbs**: never write "should" or "may" for a binding
  requirement. Use only "shall" with EARS.
- **Embedding multiple events in one `When`**: "When X happens and Y
  happens" → split.
- **Using `When` for a state**: "When the system is running" should be
  `While the system is running`.
- **Using `While` for an event**: "While a button is pressed" — if the
  behavior is on the press edge, use `When`.
- **Implicit subject**: "shall reject…" with no subject. Always name
  the system.
- **Free-text conditions**: "When the user does something appropriate,
  the system shall…". The condition must be observable and specific.
