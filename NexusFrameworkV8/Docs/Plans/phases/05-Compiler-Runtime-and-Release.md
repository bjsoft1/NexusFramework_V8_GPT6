# Phase 5 — Coherent publication, runtime consumers and release verification

**Plan:** consolidated for review. **Implementation:** NOT_STARTED. **Engine tests:** NOT_EXECUTED. **Approval:** PENDING.

Revision: V8.1-aligned five-phase documentation / 1.0 · 2026-09-10

## Goal and entry gate

Complete the versioned output pipeline and demonstrate real agent behavior against the same road/network data, with safe failure handling and measured performance.

**Entry:** Phase 4 implementation and evidence explicitly approved, with no unresolved prerequisite blocker. Earlier suites remain regression requirements.

**Editor coverage:** Verify exposes real compile/preflight/build results and runtime evidence; AI routes connects to controlled engine scenarios. Legacy references: 49–60.

**Requirements:** R02 · R03 · R04 · R07 · R08 · R09 · R10 · R11 · R12 · R13 · R15 · R16 · R17 · R18 · R19 · R20. Definitions are in [Master requirements](../01-MASTER-REQUIREMENTS.md).

## Deliverables

- A deterministic, versioned, editor-free runtime package with coherent meshes, graphs, services and manifests.
- Testable driving, walking, boarding and service consumers with explicit failure-state handling.
- Runtime state and streaming/pooling that preserve logical identity and capacity.
- Clean build/launch instructions, automated/manual evidence and a reviewer-controlled release decision.

## Twenty short tasks

1. Finalize the immutable canonical snapshot and export every required point, segment, lane, feature and service record.
2. Enforce source, authoring, catalog, schema and compiler fingerprints; reject stale/mixed inputs.
3. Produce deterministic graph/mesh/service outputs and validate them together before atomic publication.
4. Keep the last valid package on failure/cancel and replace only manifest-owned generated artifacts.
5. Support schema compatibility checks, explicit migration, stable remapping and editor-free runtime loading.
6. Implement lane-ID/distance tracking, constrained trajectories and vehicle-envelope-aware lane following.
7. Implement safe lane changes/merges with gap, prediction, braking, restriction and conflict checks.
8. Implement junction/roundabout travel, yielding, signal obedience, exit choice and legal missed-exit recovery.
9. Handle no-path, closure, stale data, blocked access and unloaded chunks through stop/wait/replan states.
10. Implement pedestrian crossing/waiting, bus boarding/alighting, seat capacity and service-access consumers.
11. Implement parking/fuel/seat reservations, occupancy, release, expiry and competing-agent ownership safely.
12. Separate immutable authored network data, mutable runtime state, instanced visuals and pooled actors.
13. Implement graph-chunk availability and actor activation/deactivation hysteresis, caps and state restoration.
14. Run continuous containment, wrong-way, adapter, intersection, crossing and service end-to-end scenarios.
15. Run reservation contention, streaming/rebasing, stale-revision, cancellation and data-loss fault tests.
16. Measure approved network/agent scale, latency, frame-time, memory and actor budgets on declared hardware.
17. Run all prior-phase regression suites, real undo/redo, recovery and independent payload round-trips.
18. Package clean extraction/build/launch/export instructions and prove them on the recorded target environment.
19. Collect actual commands, logs, captures, hashes, deviations and reviewer decisions; distinguish unexecuted tests.
20. STOP: release only after all five phase gates and explicit final approval; do not claim completion from screenshots.

## Test cases — all NOT_EXECUTED

Each case below is a scenario that may contain multiple assertions. Passing a scenario requires all its listed outcomes and applicable failure variants. The case IDs are retained in the [test matrix](../verification/TEST-MATRIX.md); no result is inferred from task completion.

| ID | Scenario | Procedure | Expected result | Requirements |
|---|---|---|---|---|
| P05-T01 | Deterministic compile | Compile identical canonical inputs twice, changing traversal order but not logical inputs. | Normalized payload hashes, IDs, lane mapping and manifests are equivalent; volatile metadata is excluded from content hashes. | R17 |
| P05-T02 | Atomic publish and stale inputs | Change source/catalog mid-build; inject errors/cancel before publication and simulate failure at switch-over. | No partial or mixed-revision output is active; last valid package remains; stale build is rejected. | R04, R17 |
| P05-T03 | Runtime completeness and schema | Load/round-trip every specialized payload and graph in runtime-only context; test supported/unknown schema versions. | All required IDs/fields survive; editor modules are absent; incompatible schema is rejected without corrupting prior data. | R09, R13, R15, R17 |
| P05-T04 | Lane following and containment | Drive configured car/bus envelopes over straight/curved/banked roads, 6→4→2 and reverse adapters. | No wrong-way/unauthorized corridor exit in the approved scenarios; record trajectory, sweep method, tolerances and failures. | R07, R08, R18 |
| P05-T05 | Lane-change/merge safety | Run valid lane-change intervals with safe and unsafe gaps, simultaneous merges and braking constraints. | Permission alone does not trigger motion; unsafe maneuvers wait/replan and approved priority is respected. | R16, R18 |
| P05-T06 | Junction and roundabout travel | Traverse T/cross/mixed-profile junctions and single/multi-lane roundabouts with missed/blocked exits. | Vehicles obey lane turns, yields and signals; legal recovery is used rather than cutting across the ring or teleporting. | R10, R11, R18 |
| P05-T07 | Crossings and signals at runtime | Execute pedestrian phases, missing/stale controller states and competing vehicle/pedestrian movements. | Conflicting permission is not granted; invalid control data invokes the documented safe policy; waiting and clearance are observed. | R12, R18 |
| P05-T08 | Runtime failure and projection | Remove an edge, close access, unload the next graph chunk and place an agent near an overpass. | Explicit stop/wait/replan behavior; no straight-line off-road fallback, uncontrolled nearest-lane jump or hidden clamping. | R16, R18, R19 |
| P05-T09 | Bus and seat consumers | Route bus entry/stop/exit and pedestrian queue/board/alight; contend for logical seats sharing benches. | Correct access/interaction poses and capacity; no double assignment; physical mesh count does not replace logical capacity. | R13, R15, R18 |
| P05-T10 | Parking and fuel service state | Compete for one bay, expire a lease, occupy/release it, and unload/reload its visual proxy. | Reservation is owner-specific and exclusive; expiry cannot clear actual occupancy; GUID-keyed state survives proxy changes. | R13, R15, R19 |
| P05-T11 | Hospital/park/fuel route integration | Drive/walk different participant classes to required service entrances and blocked/closed alternatives. | Permitted access only, including ambulance restrictions; never uses a mesh-center or placeholder route. | R13, R15, R18 |
| P05-T12 | Large-world graph and pooling | Cross chunk boundaries and rebase origin under load while decorative actors deactivate/reactivate. | Graph availability is explicit and independent of visual actors; stable IDs/state persist; hysteresis and caps hold. | R02, R19 |
| P05-T13 | Measured performance | Execute the approved world-size and active-agent fixtures with specified hardware/build settings. | Publish actual median/tail timings, memory and actor counts against agreed budgets; unresolved budget failures block scale claims. | R19, R20 |
| P05-T14 | Full regression and data safety | Rerun all earlier phase suites; include save/reopen, crash recovery, undo/redo and generator ownership checks. | No accepted scenario regresses or authored data disappears; exact failures and deviations are documented. | R04, R20 |
| P05-T15 | Clean delivery and end-to-end acceptance | Extract on the recorded clean environment; build, launch, author, undo/redo, save, compile, run and export. | Documented commands succeed with real evidence; all phase decisions and final user release approval are recorded. | R03, R17, R20 |

## Verification options and evidence

| Method | Evidence required |
|---|---|
| Automated engine/build tests | Actual command, engine/toolchain and source revision, fixture/seed, full result log, observed outcome, failure details. |
| Manual editor workflow | Reproducible steps, expected versus observed behavior, selected stable IDs, screenshots/recording and target DPI/theme. |
| Persistence/data safety | Before/after source hashes, backup path, reopened fixture, intentional-difference report and fault/recovery results. |
| Independent review | Changed files, requirement/test coverage, unresolved defects and an explicit user/reviewer decision. |

Methods complement one another. A screenshot is not proof of routing, a compile is not proof of traffic behavior, and browser simulation is not engine evidence. No test runner or Unreal project is delivered by this documentation package. Implement scenario fixtures using the [fixture catalogue](../verification/REGRESSION-FIXTURES.md).

## Boundary and dependencies

This is an editor/network/runtime-consumer deliverable, not a full commercial-game implementation or a guarantee of zero possible defects. Visual ambition must be evaluated against approved measurable criteria.

Use the same versioned record/snapshot contracts introduced in earlier phases; do not create temporary competing models. Fix a prerequisite regression before asking for gate approval. Real undo/redo, stable IDs and source-preservation rules apply to every new accepted mutation.

## Exit gate

All 20 tasks addressed; all 15 phase scenarios executed with evidence and passing, or any noncritical deviation explicitly reviewed before approval. Critical data loss, invalid driving routes, broken references or mixed-revision publication cannot be treated as acceptable polish. Record earlier-phase regressions as failures, not new scope.

Fill [Phase gate report](../verification/PHASE-GATE-REPORT-TEMPLATE.md), attach actual evidence and stop. Do not release without explicit final approval of all phase evidence and the delivery.

Current gate: `Implementation = NOT_STARTED; automated engine tests = NOT_EXECUTED; manual engine verification = NOT_EXECUTED; reviewer = PENDING`.

[Roadmap](../02-FIVE-PHASE-ROADMAP.md) · [Decision register](../decisions/DECISION-REGISTER.md) · [Start here](../00-START-HERE.md)
