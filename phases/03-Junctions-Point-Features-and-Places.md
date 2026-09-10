# Phase 3 — Shared junctions, point features and attached places

**Plan:** consolidated for review. **Implementation:** NOT_STARTED. **Engine tests:** NOT_EXECUTED. **Approval:** PENDING.

Revision: V8.1-aligned five-phase documentation / 1.0 · 2026-09-10

## Goal and entry gate

Author complete intersection and roadside-service semantics with explicit access networks, canonical shared identities and independently editable 3D handles.

**Entry:** Phase 2 implementation and evidence explicitly approved, with no unresolved prerequisite blocker. Earlier suites remain regression requirements.

**Editor coverage:** Point features and Places become functional with minimal required fields first, presets and Advanced details. Legacy references: 25–36 and the foundation of 37.

**Requirements:** R01 · R04 · R06 · R09 · R10 · R11 · R12 · R13 · R14 · R15 · R19. Definitions are in [Master requirements](../01-MASTER-REQUIREMENTS.md).

## Deliverables

- One shared record per junction/roundabout with typed approaches and explicit lane connections.
- Derived point feature switches, crossings, signal definitions and conflict validation.
- Complete bus, parking, seat, hospital, park and petrol-pump payloads with local-frame offsets and access links.
- A working generic N-sub-point 3D gizmo, not a deferred placeholder.

## Twenty short tasks

1. Implement canonical junction/roundabout records referenced by highway-point approach memberships.
2. Register T, four-way, skewed/custom and single-/multi-lane roundabout presets with versioned types.
3. Define per-approach segment frames, lane profiles, incoming/outgoing lanes and boundary compatibility.
4. Author legal per-lane turn connectors, stop/yield rules, prohibited turns and explicit U-turn policy.
5. Add ring lanes, circulation, entry yields, legal exits and unique roundabout scope membership.
6. Use one intersection transform and explicit mesh owner for ports, geometry, lights and attached offsets.
7. Derive HasCrosswalk, HasAdapter, IsJunction, IsRoundabout and HasTrafficLight from typed backing records.
8. Make feature add/remove transactional with required-field setup and reviewed dependent-link changes.
9. Implement full road-edge crosswalk span, separate sidewalk-plus-border depth, waiting points and pedestrian links.
10. Author signal heads, movement groups, controllers, phase/clearance definitions and conflict validation.
11. Place parking, bus stops, hospitals, city parks and petrol pumps under highway points.
12. Compose 3D local offsets with an explicit segment-end/approach frame; support both reparent modes.
13. Implement generic independent move/rotate gizmos for three or more array sub-points with stable IDs.
14. Complete bus entry/stop/exit, waiting, boarding/alighting, shelter/base/indicator and seat connections.
15. Complete physical/logical seat counts, capacity, spacing, grouping and independent interaction poses.
16. Complete parking grids, gaps, bay dimensions/poses/IDs, class rules and drive/walk access corridors.
17. Complete hospital entrances, park walk links and petrol-pump circulation/queues/fuel bays; never route to mesh centers.
18. Round-trip every specialized payload and test geometry, conflicts, independent handles and transactions.
19. Collect approach maps, full field inventories, mesh interfaces, engine recordings and negative-case results.
20. STOP: present Phase 3 evidence and wait for explicit approval before starting Phase 4.

## Test cases — all NOT_EXECUTED

Each case below is a scenario that may contain multiple assertions. Passing a scenario requires all its listed outcomes and applicable failure variants. The case IDs are retained in the [test matrix](../verification/TEST-MATRIX.md); no result is inferred from task completion.

| ID | Scenario | Procedure | Expected result | Requirements |
|---|---|---|---|---|
| P03-T01 | Canonical junction identity | Connect three highways to one shared junction, including a boundary between cities; rebuild in different orders. | Only one canonical junction is generated; explicit owner and scope membership remain stable. | R01, R10 |
| P03-T02 | Typed junction movements | Test T, cross, skewed and mixed-profile arms with legal/prohibited/U-turn cases. | Only approved directed lane connectors exist; invalid arms and prohibited movements are rejected. | R06, R10 |
| P03-T03 | Shared intersection transform | Rotate/bank a shared intersection at non-right angles and change its declared mesh owner. | Mesh, port frames, lane paths, signals and offsets align; ownership does not follow traversal order. | R06, R10, R14 |
| P03-T04 | Roundabout identity and schema | Reference one roundabout from four highway points; inspect ring lanes, ports, circulation and scope references. | One canonical ID is counted once per requested scope; required entry/exit and yield data are present. | R11 |
| P03-T05 | Feature-component consistency | Toggle a crossing/signal, undo/redo and import a flag that disagrees with its component links. | Switches reflect backing records; incompatible summaries or dangling dependencies cannot be saved/published silently. | R09 |
| P03-T06 | Crosswalk geometry and waiting points | Create crossings with symmetric and unequal sidewalk/border widths; choose the declared depth basis; edit waiting point 2 of 3. | Full carriageway span and explicit depth resolve correctly; no ambiguous width policy; only selected waiting handle changes. | R12, R14 |
| P03-T07 | Signal definitions and conflicts | Create incompatible greens, missing controller, clearance violations and an allowed phase. | Invalid definitions block with exact movement/control IDs; valid phase definitions serialize without mutable live state. | R12 |
| P03-T08 | Full 3D local placement | Move/rotate/bank the selected approach frame with nonzero XYZ/pitch/roll/yaw place offsets. | All services follow the full frame without drift; offsets remain local authored values. | R13, R14 |
| P03-T09 | Reparent and independent handles | Edit at least three bus waiting/seat handles; preserve-world then preserve-local reparent; cancel/undo/redo. | Siblings remain unchanged; both modes match documented math; transaction restores IDs, links and transforms. | R04, R14 |
| P03-T10 | Complete bus-stop round-trip | Save/reload and emit the complete entry/stop/exit, boarding/alighting, queue, seat and mesh-slot fixture. | Every field and typed vehicle/pedestrian connection survives; one default stop pose is not treated as complete coverage. | R13, R15 |
| P03-T11 | Physical versus logical seats | Create two benches with three logical seats each; move independent sitting poses and round-trip. | Two physical mesh instances and six logical seats retain distinct IDs, capacity and pose links. | R15 |
| P03-T12 | Parking authoring and bay access | Create a 2×6 grid with nonzero gaps, angled/accessible bays and varied class rules. | Twelve stable bay poses and explicit entry/exit/walk links serialize; dynamic occupancy is not authored state. | R13, R15 |
| P03-T13 | Hospital and city park | Set an ambulance-only entrance, ordinary access and independent park walking entrances. | Vehicle and pedestrian targets reference legal entrances, not building/park centers; restrictions persist. | R13, R15 |
| P03-T14 | Petrol pump and queues | Create entry/exit driveway circulation, fuel-bay poses, waiting points and fuel/vehicle compatibility. | Approach and exit corridors, queues and restrictions survive save/reopen with no placeholder unvalidated access IDs. | R13, R15 |
| P03-T15 | Independent payload and extension validation | Round-trip each feature/place family separately; add a registered example type and an unknown compiler type. | Known schemas preserve every required field; unknown data is retained but publication is blocked until handled. | R09, R13, R19 |

## Verification options and evidence

| Method | Evidence required |
|---|---|
| Automated engine/build tests | Actual command, engine/toolchain and source revision, fixture/seed, full result log, observed outcome, failure details. |
| Manual editor workflow | Reproducible steps, expected versus observed behavior, selected stable IDs, screenshots/recording and target DPI/theme. |
| Persistence/data safety | Before/after source hashes, backup path, reopened fixture, intentional-difference report and fault/recovery results. |
| Independent review | Changed files, requirement/test coverage, unresolved defects and an explicit user/reviewer decision. |

Methods complement one another. A screenshot is not proof of routing, a compile is not proof of traffic behavior, and browser simulation is not engine evidence. No test runner or Unreal project is delivered by this documentation package. Implement scenario fixtures using the [fixture catalogue](../verification/REGRESSION-FIXTURES.md).

## Boundary and dependencies

Real-time signal execution, reservation arbitration and agent traffic control are Phase 5 consumers. Phase 3 must already serialize/validate all inputs those consumers need.

Use the same versioned record/snapshot contracts introduced in earlier phases; do not create temporary competing models. Fix a prerequisite regression before asking for gate approval. Real undo/redo, stable IDs and source-preservation rules apply to every new accepted mutation.

## Exit gate

All 20 tasks addressed; all 15 phase scenarios executed with evidence and passing, or any noncritical deviation explicitly reviewed before approval. Critical data loss, invalid driving routes, broken references or mixed-revision publication cannot be treated as acceptable polish. Record earlier-phase regressions as failures, not new scope.

Fill [Phase gate report](../verification/PHASE-GATE-REPORT-TEMPLATE.md), attach actual evidence and stop. Do not start Phase 4 without explicit approval of this phase.

Current gate: `Implementation = NOT_STARTED; automated engine tests = NOT_EXECUTED; manual engine verification = NOT_EXECUTED; reviewer = PENDING`.

[Roadmap](../02-FIVE-PHASE-ROADMAP.md) · [Decision register](../decisions/DECISION-REGISTER.md) · [Start here](../00-START-HERE.md)
