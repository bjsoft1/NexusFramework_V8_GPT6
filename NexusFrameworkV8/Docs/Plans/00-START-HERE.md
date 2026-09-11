# NexusFramework V8 — five-phase documentation package

**V8.1-aligned five-phase documentation / 1.0**  
Prepared 2026-09-10 · **Review ready; implementation not started**

## Open and read

Extract the ZIP completely and open [START-HERE.html](START-HERE.html) for the offline document guide, or read these Markdown files directly. The guide needs no server, API key or install. The code project is not included or changed by this task.

Start with [Master requirements](01-MASTER-REQUIREMENTS.md), [Five-phase roadmap](02-FIVE-PHASE-ROADMAP.md) and [Decision register](decisions/DECISION-REGISTER.md). Then review the current five phase plans. Each contains **20 tasks, 15 planned test scenarios, verification options and an explicit stop/approval gate**.

## Current five phase files

| Phase | File | Main outcome |
|---|---|---|
| 1 | [Foundation and Landscape](phases/01-Foundation-and-Landscape.md) | Simple editor, exact hierarchy, source authority, identity, transactions and recovery. |
| 2 | [Mesh inheritance, roads and adapters](phases/02-Mesh-Inheritance-Roads-and-Adapters.md) | Four-level mesh fallback, profiles, directional 6→4→2 adapters and compatible road geometry. |
| 3 | [Junctions, features and places](phases/03-Junctions-Point-Features-and-Places.md) | Canonical intersections, crossings/signals, complete services and independent 3D handles. |
| 4 | [Tools, navigation and validation](phases/04-Editor-Tools-Navigation-and-Validation.md) | Queryable lane/service networks, roundabout discovery/exits, expert tools and actionable diagnostics. |
| 5 | [Compiler, runtime and release](phases/05-Compiler-Runtime-and-Release.md) | Coherent outputs, actual consumers, runtime state, streaming and release evidence. |

## Your key model, preserved

```text
World/Root > State > City > Highway > Highway Point > Place
Mesh fallback: Highway > City > State > Root
```

Places include parking, bus stops, hospitals, city parks and petrol pumps with point-relative location/rotation offsets and explicit access connections. Point features include adapters, crossings, signals and shared junction/roundabout memberships. Directional lane mappings and runtime safety contracts are specified, not inferred from a mesh or checkbox.

## Supporting documents

[Hierarchy/data authority](architecture/01-Hierarchy-and-Data-Ownership.md) · [Mesh/profile contract](architecture/02-Mesh-Inheritance-and-Road-Profiles.md) · [Features/places](architecture/03-Point-Features-and-Places.md) · [Runtime queries](architecture/04-Lane-Graph-and-Runtime-Contract.md) · [Editor/gizmos](architecture/05-Editor-UX-and-Gizmo-Contract.md) · [Compilation/publication](architecture/06-Compiler-Persistence-and-Publishing.md).

[Test matrix](verification/TEST-MATRIX.md) · [Requirement coverage](verification/REQUIREMENT-COVERAGE.md) · [Original 36-case mapping](verification/ENGINE-36-COVERAGE.md) · [Regression fixtures](verification/REGRESSION-FIXTURES.md) · [Gate report template](verification/PHASE-GATE-REPORT-TEMPLATE.md) · [Implementation agent instructions](AGENT-INSTRUCTIONS.md).

## References retained

The original review documents and JSON evidence are under [references](references/README.md), separate from the five current plans. The legacy V0–V7 documentation is archived there. The [simplified V8.1 HTML](prototype-reference/V8.1-Simple-Review.html) and [earlier 60-screen HTML](prototype-reference/V8-60-Screen-Reference.html) are unchanged reference copies, not new implementations. Historical screenshot/source links can require the original packages; the current guide and phase docs are self-contained.

## Approval and test status

Prototype approval: PENDING. Plan approval: PENDING. Phases 1–5: NOT_STARTED. **All 75 planned engine scenario tests: NOT_EXECUTED.** The original 36 engine requirements are mapped, not counted as additional passed tests. This ZIP does not authorize the next coding phase.

Approve prototype/plan, then implement/test/review Phase 1; continue only after explicit approval, one phase at a time. Browser/package checks cannot substitute for Unreal build and runtime verification.

The [package checks](verification/DOCUMENT-PACKAGE-CHECKS.md) concern file integrity, links, task/test counts and the documentation guide only. Use [PHASE-STATUS.json](PHASE-STATUS.json) and [PLANNED-TESTS.json](verification/PLANNED-TESTS.json) as honest initial tracking data.
