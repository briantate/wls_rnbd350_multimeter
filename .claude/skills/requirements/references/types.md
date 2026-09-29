# Requirement Types Reference

Every requirement carries a single type. Classification drives the SRS
section it lands in, the verification method that fits, and the ID
prefix it receives. Read this when you are unsure where a concern goes.

## ID prefix table

Use these prefixes consistently. Stable IDs matter — the RTM, design
docs, and tests reference them.

| Prefix | Type | Section in SRS |
|---|---|---|
| **FR-** | Functional | §3.1 Functional |
| **PR-** | Performance | §3.2 Performance |
| **IR-** | Interface (external) | §3.3 External interfaces |
| **ER-** | Environmental / physical | §3.4 Environmental & physical |
| **RR-** | Resource (memory, CPU, power) | §3.5 Resource |
| **REL-** | Reliability / availability | §3.6 Reliability |
| **SEC-** | Security | §3.7 Security |
| **UR-** | Usability / accessibility | §3.8 Usability |
| **SF-** | Safety | §3.9 Safety |
| **SM-** | States and modes | §3.10 States and modes |
| **DC-** | Design constraint | §3.11 Design constraints |

Keep IDs stable across drafts. Renumbering invalidates the RTM and any
upstream/downstream references.

## Classification decision rules

Apply in order — first match wins.

1. **Externally imposed (regulation, standard, customer-mandated tech)?**
   → **Design constraint (DC)**. Trace to the standard clause.

2. **Hazardous-state avoidance, fail-safe behavior, integrity-level
   obligation (ASIL / DAL / SIL)?** → **Safety (SF)**. Trace to hazard
   analysis or governing standard clause.

3. **Names a system mode (off, standby, init, run, faulted, calibrate)
   or a transition between modes?** → **States and modes (SM)**.

4. **Names something at a system boundary** — protocol, signal, pin,
   API, message format, file format, wire format?
   → **Interface (IR)**.

5. **Quantified speed, throughput, latency, capacity, response time,
   sample period, jitter?** → **Performance (PR)**.

6. **Names an operating-environment condition** (temperature,
   vibration, humidity, EMI/EMC, mounting, mass, dimensions)?
   → **Environmental / physical (ER)**.

7. **Names a budget the system lives within** — RAM, flash, stack,
   CPU%, bandwidth, power, battery life?
   → **Resource (RR)**.

8. **Names uptime, MTBF, fault tolerance, graceful degradation, data
   integrity over time?** → **Reliability (REL)**.

9. **Names authn / authz, integrity, confidentiality, audit, crypto,
   threat-model coverage?** → **Security (SEC)**.

10. **Names human-factors target, accessibility, ease-of-use, WCAG
    conformance?** → **Usability (UR)**.

11. **Otherwise, a behavior the system performs in response to inputs
    or events** → **Functional (FR)**.

If two rules seem to fit, prefer the more specific (Safety beats
Functional; Performance beats Functional; Interface beats Performance
when the concern is about the boundary, not the speed).

## Type-by-type guidance

### Functional (FR)

What the system *does*. Inputs → behavior → outputs. The largest section
in most SRSes.

- Default EARS pattern: **event-driven** (`When …, the <system> shall …`).
- Default verification: **Test**.
- Map directly from GWT acceptance criteria; one AC scenario typically
  yields one functional requirement.

**Example:**
> **FR-014.** When an update is invoked with a temperature reading
> below `(setpoint − hysteresis_band/2)`, the temperature controller
> shall assert the heater output before the update returns.
> *Verified by: Test. Parent: 006/AC-1.*

### Performance (PR)

Quantified time, throughput, latency, capacity. Performance is *not* a
NFR family — it is its own section because the verification approach is
distinct (instrumented test vs. functional test).

- Default EARS pattern: **ubiquitous** or **event-driven**.
- Default verification: **Test** with timing instrumentation, or
  **Analysis** for worst-case proofs.
- Always quantified with units. "Fast" is never a performance
  requirement.

**Example:**
> **PR-002.** The temperature controller's `update` operation shall
> complete within 5 ms on the reference target.
> *Verified by: Test. Parent: 004/DoD WCET item.*

### Interface (IR)

Characteristics at the system boundary. Protocol, signal, pin
assignment, message format, API contract, file format.

- Default EARS pattern: **ubiquitous** for invariants; **event-driven**
  for stimulus-response on the boundary.
- Default verification: **Inspection** (for static contracts) or
  **Test** (for dynamic protocols).
- For embedded: name the physical interface (I2C address, GPIO pin,
  byte order, bit ordering).

**Example:**
> **IR-001.** The temperature controller shall accept temperature
> readings as IEEE-754 single-precision floats in degrees Celsius
> through the `update(controller_t*, float, uint32_t)` operation.
> *Verified by: Inspection. Parent: 004/AC-1.*

### Environmental / physical (ER)

Operating conditions and physical constraints.

- Default verification: **Test** (environmental chamber) or
  **Analysis**.

**Example:**
> **ER-001.** The temperature controller shall operate correctly across
> ambient temperatures of 0 °C to 60 °C and relative humidity 0–95%
> non-condensing.
> *Verified by: Test. Parent: product-level cross-cutting.*

### Resource (RR)

Memory, CPU, power, bandwidth budgets.

- Default verification: **Analysis** for static budgets (flash, RAM
  size); **Test** for dynamic budgets (CPU%, peak power).

**Example:**
> **RR-001.** The temperature controller's static code footprint shall
> not exceed 32 kB of flash.
> *Verified by: Analysis. Parent: product-level cross-cutting.*

### Reliability (REL)

Availability, fault tolerance, graceful degradation, MTBF.

- Default verification: **Test** (fault injection) or **Analysis** (FMEA).

**Example:**
> **REL-001.** While the temperature controller is in sensor-fault safe
> state, the temperature controller shall maintain the heater output
> de-asserted until a non-faulted reading is supplied.
> *Verified by: Test. Parent: 009.*

### Security (SEC)

Authn / authz / integrity / confidentiality / audit.

- Default verification: **Test** + **Analysis** (threat model).
- Where standards apply (e.g., FIPS, OWASP), trace to the clause.

### Usability (UR)

Human-factors targets. Often quantified by task completion time, error
rate, accessibility conformance.

- Default verification: **Demonstration** + **Test** (user studies).
- Avoid "intuitive" / "user-friendly". Quantify.

### Safety (SF)

Hazardous-state avoidance and fail-safe obligations. In safety-critical
domains these are the highest-rigor requirements.

- Default verification: **Test** + **Analysis** (FMEA, fault-tree).
- Carries an integrity level (ASIL A–D, DAL A–E, SIL 1–4) when the
  domain mandates it.
- Trace to hazard analysis as well as to the originating story.

**Example:**
> **SF-001.** *(ASIL B)* When the temperature reading at update is at
> or above the configured over-temperature threshold, the temperature
> controller shall de-assert the heater output before the update
> returns and shall raise an over-temperature fault event.
> *Verified by: Test + Analysis. Parent: 008, ISO 26262 hazard H-3.*

### States and modes (SM)

Required system modes and the transitions between them.

- Default EARS pattern: **state-driven** for behavior within a mode;
  **event-driven** for transitions.
- Default verification: **Demonstration** + **Test**.
- A state diagram or state table belongs in the SRS introduction or as
  a figure adjacent to this section.

**Example:**
> **SM-002.** When the controller is in `idle` mode and a valid
> initialization completes, the temperature controller shall transition
> to `regulating` mode.
> *Verified by: Test. Parent: 003.*

### Design constraint (DC)

Externally imposed obligations: regulatory, certifying,
standards-mandated, customer-mandated. **Not** team design choices.

- Default verification: **Inspection**.
- Always traces to a clause of the inherited document.

**Example:**
> **DC-001.** The temperature controller source shall comply with
> MISRA-C:2012 mandatory and required rules. Documented deviations
> shall be recorded in the project deviation register.
> *Verified by: Inspection. Parent: project quality plan §4.2.*

## Common misclassifications

- **"Logging" → SEC, not FR.** Logging is usually an audit/security
  obligation, not a feature.
- **"Boot time" → PR, not FR.** It is a quantified time target.
- **"Configurable via a table" → FR (functional capability), not DC.**
  The configurability is a feature, not an externally imposed
  constraint.
- **"No dynamic allocation" → DC (design constraint inherited from
  embedded coding standard / safety norm), not FR.** It is a constraint
  on implementation, not a behavior.
- **"Deterministic update timing" → PR (performance), not REL.**
  Determinism is quantified by jitter and WCET.
- **"Safe state on fault" → SF (safety), not REL.** Fail-safe is the
  defining characteristic of safety, not reliability. Reliability is
  about *availability over time*; safety is about *avoiding
  hazardous states*.
