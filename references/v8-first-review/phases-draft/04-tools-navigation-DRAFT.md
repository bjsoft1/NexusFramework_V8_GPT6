# Phase 4 — Advanced editor tools, diagnostics & navigation contracts

**Status: PROVISIONAL DRAFT — NOT APPROVED — NOT STARTED.**

These are 20 review points, not authorization to start implementation. First approve the 60-screen prototype, then finalize all five phase plans. Real implementation runs sequentially and stops at each gate. Existing V0–V7 phase numbers do not carry forward into this new five-phase plan.

**Prototype coverage:** 37–48.
**Entry condition:** Phase 3 completed, evidence reviewed, and explicitly approved.

## Twenty short implementation and verification points

1. Polish the generic gizmo with local/world space, axis constraints, snapping, and explicit drag cancellation.
2. Unify viewport selection, hierarchy selection, Details focus, and stable N-sub-point identity.
3. Implement atomic batch transform and property edits with affected/skipped counts and before/after previews.
4. Prevent tool, camera, native spline visualizer, and dockable-tab lifecycle conflicts.
5. Add seeded roadside placement for city lights, signs, barriers, bins, and registered service fixtures.
6. Implement the activation registry contract with persistence, mesh-slot, gizmo, validation, and export adapters.
7. Implement always-draw diagnostics without introducing an alternative geometry source or duplicate graph.
8. Implement diagnostic LOD, impostors, distance culling, caps, and selected-item priority.
9. Implement actionable validation issues, entity focus, transactional quick fixes, and hard blocking gates.
10. Build GUID-based highway connectivity and preserve ConnectedHighwayIds in serialized runtime views.
11. Build directed lane graphs with samples, widths, speed/access rules, lateral neighbors, and legal maneuvers.
12. Build pedestrian/service connectivity and typed drive/walk/bus/parking route-query contracts.
13. Test move/rotate/scale, snapping, local/world conversion, cancel, batch undo/redo, and selection reconciliation.
14. Test tools while changing mode, tabs, levels, selection, camera focus, and native spline editing.
15. Test debug caps, near/far rendering, always-draw updates, and selected-item priority on large fixtures.
16. Test ConnectedHighwayIds round-trip, shared junctions, dead ends, loops, prohibited turns, and no-path results.
17. Test lane-change intervals, opposing lanes, vehicle-class restrictions, and pedestrian accessibility.
18. Test registry extension and unknown payload behavior without silently dropping unrecognized source data.
19. Collect graph fixtures, profiling evidence, representative datasets, lifecycle recordings, and failure cases.
20. STOP: review Phase 4 evidence and obtain explicit approval before implementing Phase 5.

## Required test cases — all UNEXECUTED

| ID | Test | Procedure | Expected result |
|---|---|---|---|
| P4-T01 | Generic tool lifecycle | Switch mode/tab/selection during active drag; cancel and restart. | No stuck capture, accidental camera transform, or lost selection. |
| P4-T02 | Batch atomicity | Edit three services and include a read-only source point. | Exact included/skipped counts; one accepted transaction; source point unchanged. |
| P4-T03 | Always draw | Leave Landscape Mode; modify selection; open another tab. | Diagnostics continue to refresh from the shared graph. |
| P4-T04 | Debug budgets | Load large representative data; move camera across near/far thresholds. | Caps enforced; selected diagnostics retained; measurements documented. |
| P4-T05 | Connectivity serialization | Serialize and reload highway and lane graph fixtures. | ConnectedHighwayIds and all lane references remain valid. |
| P4-T06 | Routing restrictions | Query bus, car, pedestrian, and inaccessible destinations. | Legal paths only; explicit typed failure for no valid path. |
| P4-T07 | Lane changes | Test dashed/solid boundaries, opposing direction, and junction areas. | Only permitted intervals compile; runtime safety remains a separate check. |
| P4-T08 | Registry extension | Register advertisement/TV payload; save/reload; test unknown future type. | Adapters round-trip; unsupported compilation blocks without deleting data. |

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
