---
name: adr-best-practices
description: Best practices for writing Architectural Decision Records
disable-model-invocation: true
user-invocable: false
---

# ADR Best Practices

## What is an ADR?

An Architecture Decision Record (ADR) captures the reasoning behind significant
architectural decisions. It provides transparency in decision-making and creates
a historical record for current and future team members.

## ADR Lifecycle

- **Proposed**: Under discussion, open for feedback
- **Accepted**: Approved and in effect
- **Deprecated**: No longer recommended but may still be in use
- **Superseded by ADR-XXXX**: Replaced by a newer decision

## Required Sections

1. **Title**: Short, problem-and-solution focused (e.g., "Use PostgreSQL for User Data")
2. **Status**: Current lifecycle state with metadata
3. **Context**: 2-3 sentences explaining the situation, constraints, and why a decision is needed
4. **Decision**: Clear statement of what was chosen and why
5. **Consequences**: Both positive and negative impacts
6. **Alternatives Considered**: Other options evaluated with reasons for rejection
7. **Compliance**: How the decision will be verified (CI checks, reviews, automation)
8. **Related ADRs**: Cross-references to dependent or related decisions

## Quality Checklist

Before finalizing an ADR, verify:
- [ ] Title is clear and action-oriented
- [ ] Context explains the problem with specific details
- [ ] Decision is a clear statement, not a question
- [ ] All realistic alternatives are documented with trade-offs
- [ ] Consequences include BOTH positive and negative impacts
- [ ] Stakeholders are identified
- [ ] Compliance mechanism is defined (prefer automation)
- [ ] Related ADRs are cross-referenced

## Writing Tips

**DO:**
- Use straightforward language accessible to all team members
- Provide sufficient detail about the problem BEFORE presenting the solution
- Explain WHY something was chosen, not just WHAT
- Record decisions while context is fresh
- Include tables, diagrams, or code snippets when helpful

**DON'T:**
- Use vague justifications ("it's better", "industry standard")
- Skip documenting rejected alternatives
- Leave consequences section empty or one-sided
- Write ADRs after the fact when context is forgotten

## Directory Organization

Organize ADRs by category with numbered prefixes:
```
adr/
├── 0000-record-template.md       # Template
├── 00-shared/                    # Cross-cutting (0001-0999)
│   ├── 0001-record-architecture-decisions.md
│   └── 0002-design-principles.md
├── 01-embedded/                  # Domain-specific (1000-1999)
│   ├── 1001-no-mcu.md
│   └── 1002-rtos-architecture.md
└── 02-cloud/                     # Another domain (2000-2999)
```

## Numbering Convention

- `0001-0999`: Shared/global decisions
- `1000-1999`: First domain (e.g., embedded)
- `2000-2999`: Second domain (e.g., cloud)
- `3000+`: Additional domains as needed

## Team Adoption

- Talk about the "why" together rather than mandating the "what"
- Start small with meaningful decisions before expanding scope
- Make ADRs part of design reviews and merge requests
- Periodically revisit ADRs to ensure they remain relevant

## References

- [MADR Template](https://adr.github.io/madr/)
- [ADR GitHub Organization](https://adr.github.io/)
- [Michael Nygard's Original ADR Article](http://thinkrelevance.com/blog/2011/11/15/documenting-architecture-decisions)
- [AWS ADR Best Practices](https://docs.aws.amazon.com/prescriptive-guidance/latest/architectural-decision-records/adr-process.html)
