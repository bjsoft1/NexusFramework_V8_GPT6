# Phase 5 — Canonical compiler, runtime consumers & release verification

**Status: PROVISIONAL DRAFT — NOT APPROVED — NOT STARTED.**

These are 20 review points, not authorization to start implementation. First approve the 60-screen prototype, then finalize all five phase plans. Real implementation runs sequentially and stops at each gate. Existing V0–V7 phase numbers do not carry forward into this new five-phase plan.

**Prototype coverage:** 49–60.
**Entry condition:** Phase 4 completed, evidence reviewed, and explicitly approved.

## Twenty short implementation and verification points

1. Compile one immutable canonical runtime snapshot from validated geometry, semantics, and catalog resolution.
2. Enforce source/catalog/authoring fingerprints and reject stale or inconsistent inputs before publication.
3. Use deterministic ordering, versioned schema, migration checks, and staged atomic output promotion.
4. Export points, segments, tangents, lanes, widths, sidewalks, crossings, signals, junction links, and highway connectivity.
5. Export complete bus-stop, parking, seat, and custom activation contracts without editor-module dependencies.
6. Build ownership manifests and incremental mesh chunks; replace only outputs belonging to the generator.
7. Implement vehicle travel consumers for lane following, legal changes, junctions, dead ends, and route failures.
8. Implement pedestrian walking, waiting, boarding, alighting, and seat/service interactions.
9. Implement runtime parking reservation, occupancy, release, contention, expiry, and agent ownership.
10. Separate immutable world records, mutable runtime service state, instanced static meshes, and pooled actors.
11. Implement World Partition-aware activation/deactivation hysteresis, actor caps, and GUID-keyed state restoration.
12. Package the project and export artifacts with manifests, schema versions, validation reports, and clear setup steps.
13. Test deterministic compile, stale input rejection, injected failure, cancellation, and prior-output preservation.
14. Test all runtime payloads separately; include independent Bus Stop, Parking, and Seat round-trip suites.
15. Test lane following, turns, class restrictions, permitted lane changes, collision/gap decisions, and no-path recovery.
16. Test pedestrian/boarding capacity and parking reservation races, expiry, occupancy, and proxy unload/reload.
17. Test streaming transitions, logical/actor count separation, budget enforcement, and runtime dependency isolation.
18. Test clean installation/build, editor launch, packaged runtime launch, undo/redo regression, save/restart, and the author-to-runtime workflow.
19. Collect real test reports, machine specifications, profiling captures, schema hashes, and approved known limitations.
20. STOP: release only after all five gates pass and you explicitly approve the final project and evidence.

## Required test cases — all UNEXECUTED

| ID | Test | Procedure | Expected result |
|---|---|---|---|
| P5-T01 | Compiler reproducibility | Compile equal canonical inputs twice; compare normalized payload hashes. | Identical deterministic output; timestamps excluded from canonical hash. |
| P5-T02 | Failure / cancel safety | Inject failure before atomic publish and cancel during generation. | Last valid package survives; no partial output is promoted. |
| P5-T03 | Runtime completeness | Round-trip each specialized payload and connected graph separately. | All required fields and GUID references preserved; editor-free load. |
| P5-T04 | Vehicle consumers | Travel through straight, curve, T, cross, merge, loop, and dead-end fixtures. | Legal lane use, correct turns, explicit no-path handling, safe maneuver checks. |
| P5-T05 | Services | Walk/queue/board/alight; reserve/occupy/release bay; contest same reservation. | Capacity enforced and exclusive reservations remain consistent. |
| P5-T06 | Pooling continuity | Unload/reload occupied bay and reserved seat proxies while agents remain logical. | GUID-keyed state survives; no duplicate physical actors. |
| P5-T07 | Streaming scale | Cross partitions under representative load with specified hardware. | Documented actor caps and hysteresis; measured budgets, not invented targets. |
| P5-T08 | Clean end-to-end | Extract release on a clean environment; build; author; undo/redo; save/reopen; compile; run. | Documented setup succeeds with repeatable evidence and no source data loss. |

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
