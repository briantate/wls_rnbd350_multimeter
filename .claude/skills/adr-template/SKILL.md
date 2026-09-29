---
name: adr-template
description: Template for creating new ADRs
disable-model-invocation: true
user-invocable: false
---

# ADR Template

Use this template when creating new Architectural Decision Records.

## Template

```markdown
# [ADR-XXXX] Title of Decision

**Status**: Proposed | Accepted | Deprecated | Superseded by ADR-XXXX
**Last Updated**: YYYY-MM-DD
**Author**: Author Name
**Approved by**: Approver Name

## Context

Explain the background and why a decision is being made. Include:
- Business requirements or technical challenges
- Constraints (time, budget, team skills, existing systems)
- Forces driving the need for a decision

## Decision

Clearly describe the decision made and justify why. Include:
- What was chosen
- Key reasons for the choice
- How it addresses the context

## Consequences

Describe the effects of this decision:

**Positive:**
- Benefit 1
- Benefit 2

**Negative:**
- Trade-off 1
- Trade-off 2

## Alternatives Considered

### Option A: [Name]
Brief description. Rejected because...

### Option B: [Name]
Brief description. Rejected because...

## Compliance

Explain how compliance for this ADR will be achieved:
- CI checks
- Static analysis
- Code review criteria
- Automated testing
- Traceability reports

Preference is for automation wherever possible.

## Related ADRs

- [ADR-XXXX: Related Decision](./path/to/adr.md)
- [ADR-YYYY: Another Related Decision](./path/to/adr.md)
```
