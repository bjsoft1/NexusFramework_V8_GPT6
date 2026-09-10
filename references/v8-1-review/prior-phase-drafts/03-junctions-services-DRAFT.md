# Phase 3 — Junctions, signals, bus stops, seats & parking

**Status: PROVISIONAL DRAFT — NOT APPROVED — NOT STARTED.**

These are 20 review points, not authorization to start implementation. First approve the 60-screen prototype, then finalize all five phase plans. Real implementation runs sequentially and stops at each gate. Existing V0–V7 phase numbers do not carry forward into this new five-phase plan.

**Prototype coverage:** 25–36, Generic gizmo foundation for screen 37.
**Entry condition:** Phase 2 completed, evidence reviewed, and explicitly approved.

## Twenty short implementation and verification points

1. Implement T-junction, four-way, and roundabout topology using explicit connected source branches.
2. Use one shared junction transform for meshes, lane sockets, signal heads, and activation offsets.
3. Generate smooth corners, short modular arms, compatible sidewalks, and clean conflict-area markings.
4. Implement explicit lane-to-lane junction connectors, prohibited turns, roundabout yields, and merge rules.
5. Implement traffic-light pole/head placement, lane groups, phase definitions, and conflict validation.
6. Implement crosswalk endpoints, full depth/span, sidewalk connections, signal links, and waiting arrays.
7. Land the generic N-sub-point gizmo foundation here so every service is editable before Phase 4 polish.
8. Implement bus entry, stopping, exit, boarding, alighting, waiting, and lane relationships.
9. Preserve bus Shelter, BaseMesh, IndicatorMesh, and seat-chair capabilities through resolved catalog slots.
10. Implement physical seat count, logical capacity, spacing, groups, poses, and interaction points.
11. Implement parking rows, columns, row/column gaps, bay dimensions, rotation, vehicle classes, and mesh slots.
12. Implement parking entry/exit paths, pedestrian exits, accessible bays, and separate runtime-state identifiers.
13. Test junction rotation updates geometry and linked offsets consistently without changing ownership.
14. Test signal conflicts, prohibited turns, crossing clearances, and missing controller references.
15. Test N ≥ 3 independent Crosswalk and Bus Stop waiting handles with move, rotate, cancel, undo, and redo.
16. Test a complete Bus Stop payload round-trip including all component slots and seat interaction links.
17. Test Parking independently: dimensions, spacing, bays, access, mesh slots, and occupancy-query identifiers.
18. Test Seat grouping and capacity independently; multiple logical seats may share one physical bench.
19. Collect payload field inventories, save/reload fixtures, mesh boundary screenshots, and interaction recordings.
20. STOP: review Phase 3 evidence and obtain explicit approval before implementing Phase 4.

## Required test cases — all UNEXECUTED

| ID | Test | Procedure | Expected result |
|---|---|---|---|
| P3-T01 | Junction rotation | Rotate junction by several non-right angles, undo/redo, then reopen. | Mesh, lane connectors, signal heads, and sub-points remain aligned. |
| P3-T02 | Signal conflicts | Enable incompatible greens in the same phase. | Blocking conflict with exact movement-group identifiers. |
| P3-T03 | Crosswalk N=3 | Move waiting point 2 of 3; rotate, cancel, undo/redo. | Only selected GUID changes; cancellation restores start state. |
| P3-T04 | Bus Stop N=3 | Repeat independently on three passenger waiting points. | All points retain their own position and rotation. |
| P3-T05 | Bus payload | Save/reload/compile the full bus-stop fixture. | Shelter/base/indicator/seat fields and route links are preserved. |
| P3-T06 | Parking payload | Round-trip a 2×6 layout with nonzero gaps and different bay rules. | Bay transforms and IDs match; entry/exit and mesh slots survive. |
| P3-T07 | Seat payload | Use two benches with three logical seats each. | Two meshes, six logical seats, correct independent poses and capacity. |
| P3-T08 | Lane connectors | Run T, cross, roundabout, grade-separated and merge fixtures. | All legal edges exist; prohibited or false intersection edges do not. |

## Verification options

| Method | Required evidence |
|---|---|
| Automated | Named test IDs, executed command, build/schema version, actual result, output log. |
| Manual editor review | Reproduction steps, expected/observed behavior, selected object IDs, screenshot or recording. |
| Persistence / data safety | Before/after source hashes, reopened data fixtures, interruption/failure result. |
| Independent review | Implementation diff, unresolved issues, signed or explicitly recorded reviewer decision. |

## Exit gate

`Implementation = Not started; automated tests = Not executed; manual verification = Not executed; reviewer decision = Pending.`

A gate is not satisfied by screenshots alone, by a browser simulation, by a successful build alone, or by an AI statement of completion. Report exact failures and affected requirements. No next-phase implementation starts until you explicitly approve this phase.
