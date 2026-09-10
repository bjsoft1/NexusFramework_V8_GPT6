# V8 five-phase roadmap

**Five active phases · 100 tasks · 75 planned scenario tests.** All engine tests remain NOT_EXECUTED and all phase approvals remain PENDING.

| Phase | Scope and task file | Tasks | Test scenarios | Implementation / approval |
|---|---|---:|---:|---|
| 1 | [Foundation, simple editor and Landscape authority](phases/01-Foundation-and-Landscape.md) | 20 | 15 | NOT_STARTED / PENDING |
| 2 | [Mesh inheritance, road profiles and directional adapters](phases/02-Mesh-Inheritance-Roads-and-Adapters.md) | 20 | 15 | NOT_STARTED / PENDING |
| 3 | [Shared junctions, point features and attached places](phases/03-Junctions-Point-Features-and-Places.md) | 20 | 15 | NOT_STARTED / PENDING |
| 4 | [Advanced tools, lane queries and actionable validation](phases/04-Editor-Tools-Navigation-and-Validation.md) | 20 | 15 | NOT_STARTED / PENDING |
| 5 | [Coherent publication, runtime consumers and release verification](phases/05-Compiler-Runtime-and-Release.md) | 20 | 15 | NOT_STARTED / PENDING |

## Phase dependency ladder

Phase 1 establishes identity, transactions, source bindings, type registry and a minimal snapshot. Phase 2 extends the same model to mesh resolution, profiles and directional adapters. Phase 3 adds canonical intersections, typed features and fully specified places with working 3D gizmos. Phase 4 supplies complete graph queries, validation and expert tools. Phase 5 finishes coherent runtime publication, agents, mutable state and release evidence.

This is incremental delivery, not five disconnected implementations. A later-phase feature can have an early interface/fixture, but its UI must not falsely imply that production behavior already works.

## Review and authorization

The request to assemble this ZIP authorizes documentation work only. Record prototype and plan approval before implementation. At every phase, implement, execute tests, capture actual evidence and stop for explicit review. There is no automatic advancement after elapsed time, a build, an agent's declaration or a historical browser test.

## Revised scope compared with the first five drafts

| Phase | Important consolidation |
|---|---|
| 1 | Full hierarchy through point-attached place identities; six-workspace UI; independent segment-end tangent bindings and safe source reconciliation. |
| 2 | Per-slot fallback with broken-asset diagnostics; before/after profile ownership; explicit forward/reverse adapters and shared mesh-owner contracts. |
| 3 | One canonical junction/roundabout, typed point features, hospital/park/fuel access, full bus/parking/seat payloads and working 3D sub-point gizmos. |
| 4 | Unique roundabout and entry-relative exit queries; legal lane changes versus adjacency; service reachability; typed failure states and bounded projection. |
| 5 | Mesh/graph revision coherence, real controller safety, service state, graph availability, streaming, repeatable release and measured performance. |

## Scope boundaries

This package contains documents and unchanged browser references, not a new C++/C# project. Historical code trees and browser success reports do not establish V8 engine compatibility or completed functionality. Exact engine/toolchain, budgets and proposed design choices are tracked in [Decision register](decisions/DECISION-REGISTER.md).

[Master requirements](01-MASTER-REQUIREMENTS.md) · [Test matrix](verification/TEST-MATRIX.md) · [Phase status](PHASE-STATUS.json)
