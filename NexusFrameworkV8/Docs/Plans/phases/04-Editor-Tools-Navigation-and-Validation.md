# Phase 4 — Advanced tools, lane queries and actionable validation

**Plan:** consolidated for review. **Implementation:** NOT_STARTED. **Engine tests:** NOT_EXECUTED. **Approval:** PENDING.

Revision: V8.1-aligned five-phase documentation / 1.0 · 2026-09-10

## Goal and entry gate

Make all authored networks queryable and diagnosable while keeping ordinary authoring simple and making expert tools robust.

**Entry:** Phase 3 implementation and evidence explicitly approved, with no unresolved prerequisite blocker. Earlier suites remain regression requirements.

**Editor coverage:** AI routes and Verify become functional; World/Point features/Places gain optional Advanced tools without permanent extra panels. Legacy references: 37–48.

**Requirements:** R02 · R03 · R04 · R08 · R09 · R10 · R11 · R13 · R14 · R15 · R16 · R17 · R19 · R20. Definitions are in [Master requirements](../01-MASTER-REQUIREMENTS.md).

## Deliverables

- Lane, junction, roundabout, pedestrian and service query APIs with explicit failure states.
- Safe selection, batch tools, 3D gizmo refinement and controlled debug visualization.
- Selectable diagnostics and a negative-case validation suite, with graph/schema round-trips.

## Twenty short tasks

1. Extend the shared snapshot compiler into directed lane, pedestrian and service graphs, not a separate geometry model.
2. Build stable lane/highway indexes, predecessor/successor links and ConnectedHighwayIds exports.
3. Separate lateral adjacency from legal lane-change intervals and participant-specific permission.
4. Implement lane/distance queries with elevation, heading and revision-aware bounded projection.
5. Implement route and reachable-place queries using explicit entrance/access paths rather than mesh centers.
6. Implement junction movement queries with incoming-lane restrictions, controller links and current rule context.
7. Implement unique roundabout counts and entry-relative legal exit ordering with explicit U-turn policy.
8. Expose upcoming features, service access and pedestrian accessibility through typed query results.
9. Return explicit no-path, closed, stale, invalid and unloaded states; never invent straight-line fallback.
10. Polish local/world gizmos, constraints, snapping, allowed scale fields and cancel behavior.
11. Unify viewport/tree/details selection and implement atomic batch edits with affected/skipped previews.
12. Prevent tab, mode, camera, native spline visualizer and active-drag lifecycle conflicts.
13. Add deterministic roadside placement for registered signs, lights, barriers and fixtures.
14. Prove type-registry extension across editor fields, mesh roles, persistence, gizmos, validation and export.
15. Add optional always-draw diagnostics with culling, LOD, caps and selected-entity priority.
16. Provide actionable issue selection, provenance, transactional fixes and hard publication blockers.
17. Run directed-route, adapter, junction, roundabout, service-access and grade-separation negative tests.
18. Run gizmo, batch, lifecycle, debug-budget and final Basic/Advanced usability checks.
19. Collect query fixtures, route overlays, commands, profiling results and regression evidence.
20. STOP: present Phase 4 evidence and wait for explicit approval before starting Phase 5.

## Test cases — all NOT_EXECUTED

Each case below is a scenario that may contain multiple assertions. Passing a scenario requires all its listed outcomes and applicable failure variants. The case IDs are retained in the [test matrix](../verification/TEST-MATRIX.md); no result is inferred from task completion.

| ID | Scenario | Procedure | Expected result | Requirements |
|---|---|---|---|---|
| P04-T01 | Connectivity serialization | Round-trip highway/lane graphs including shared junctions, loops and dead ends. | Stable IDs, ConnectedHighwayIds and legal directed edges survive; no actor pointer is runtime identity. | R10, R16 |
| P04-T02 | Lane-change permission | Query dashed/solid boundaries, opposing lanes, bus restrictions and prohibited junction intervals. | Adjacent but prohibited lanes are not returned as legal changes; interval and participant data are explicit. | R16 |
| P04-T03 | Directional adapter routes | Find routes through 6→4→2 and reverse; remove a merge link and restrict one participant class. | All allowed routes use explicit mapping; invalid route returns NO_PATH with diagnostic context. | R08, R16 |
| P04-T04 | Junction query rules | Query each approach lane at mixed-profile T/cross/skewed junctions. | Only legal turns are returned; turn bans, class rules and U-turn policy are honored. | R10, R16 |
| P04-T05 | Unique roundabout counts | Query world/city scopes with four references to one roundabout and an intersection shared across cities. | One canonical ID appears once within each selected scope; boundary-membership policy is documented. | R11 |
| P04-T06 | Entry-relative roundabout exits | Query single/multi-lane ring routes using both circulation directions, restricted exits and U-turn modes. | Physical port count is separate from legal onward exits; entry-relative ordering and reachability are correct. | R11, R16 |
| P04-T07 | Service and walk reachability | Query each parking bay, bus stop, hospital, park and fuel entrance for cars/buses/pedestrians. | Routes terminate at permitted access/interaction records; inaccessible destinations return typed failure. | R13, R15, R16 |
| P04-T08 | Projection and grade separation | Project near parallel/stacked roads and far from the network using heading/elevation constraints. | No nearest-road jump across elevation/direction; out-of-bounds queries are explicit failures. | R02, R16 |
| P04-T09 | Failure-state contract | Request missing IDs, closed exit, unloaded graph chunk and stale snapshot revision. | Returns distinguish invalid, NO_PATH, CLOSED, DATA_UNAVAILABLE and STALE; no successful fake route. | R16, R17 |
| P04-T10 | Gizmo and batch atomicity | Move/rotate/allowed-scale multiple objects, include a read-only point, then cancel/undo/redo. | Correct include/skip preview; one accepted transaction; source geometry and sibling handles remain unchanged. | R04, R14 |
| P04-T11 | Tool lifecycle | Change mode, map, tab and selection during drag; invoke camera and native spline tools. | No stuck capture, transform fighting, stale callbacks or lost/incorrect selection. | R03, R14 |
| P04-T12 | Debug budgets and always-draw | Run declared large fixtures across near/far thresholds with modes/tabs changed. | Caps/LOD/culling work; selected diagnostics remain useful; cost is measured on declared hardware. | R03, R19 |
| P04-T13 | Actionable validation | Inject orphan link, bad mesh port, unresolved successor and conflicting signal; invoke Select and allowed Fix. | Exact offending entities are selected; accepted fixes are transactional; blockers remain until genuinely resolved. | R09, R16, R17 |
| P04-T14 | Registered extension and deterministic placement | Register an advertisement/TV or approved new type; place by fixed seed; repeat build; try unknown type. | Known type uses all adapters and stable placement; unknown export blocks without deleting authored data. | R19 |
| P04-T15 | Expert and Basic usability | Complete common tasks without Advanced; use keyboard-only expert tools at 1080p and scaled DPI in both themes. | Primary controls stay reachable and labeled; advanced capability does not become mandatory for routine authoring. | R03, R20 |

## Verification options and evidence

| Method | Evidence required |
|---|---|
| Automated engine/build tests | Actual command, engine/toolchain and source revision, fixture/seed, full result log, observed outcome, failure details. |
| Manual editor workflow | Reproducible steps, expected versus observed behavior, selected stable IDs, screenshots/recording and target DPI/theme. |
| Persistence/data safety | Before/after source hashes, backup path, reopened fixture, intentional-difference report and fault/recovery results. |
| Independent review | Changed files, requirement/test coverage, unresolved defects and an explicit user/reviewer decision. |

Methods complement one another. A screenshot is not proof of routing, a compile is not proof of traffic behavior, and browser simulation is not engine evidence. No test runner or Unreal project is delivered by this documentation package. Implement scenario fixtures using the [fixture catalogue](../verification/REGRESSION-FIXTURES.md).

## Boundary and dependencies

Static legal routes and lane-change permissions are not real-time safety decisions. Full moving-agent controllers, live closures/reservations and release-scale tests complete in Phase 5.

Use the same versioned record/snapshot contracts introduced in earlier phases; do not create temporary competing models. Fix a prerequisite regression before asking for gate approval. Real undo/redo, stable IDs and source-preservation rules apply to every new accepted mutation.

## Exit gate

All 20 tasks addressed; all 15 phase scenarios executed with evidence and passing, or any noncritical deviation explicitly reviewed before approval. Critical data loss, invalid driving routes, broken references or mixed-revision publication cannot be treated as acceptable polish. Record earlier-phase regressions as failures, not new scope.

Fill [Phase gate report](../verification/PHASE-GATE-REPORT-TEMPLATE.md), attach actual evidence and stop. Do not start Phase 5 without explicit approval of this phase.

Current gate: `Implementation = NOT_STARTED; automated engine tests = NOT_EXECUTED; manual engine verification = NOT_EXECUTED; reviewer = PENDING`.

[Roadmap](../02-FIVE-PHASE-ROADMAP.md) · [Decision register](../decisions/DECISION-REGISTER.md) · [Start here](../00-START-HERE.md)
