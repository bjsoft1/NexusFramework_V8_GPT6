# Phase 2 — Mesh inheritance, road profiles and directional adapters

**Plan:** consolidated for review. **Implementation:** NOT_STARTED. **Engine tests:** NOT_EXECUTED. **Approval:** PENDING.

Revision: V8.1-aligned five-phase documentation / 1.0 · 2026-09-10

## Goal and entry gate

Resolve existing mesh configurations predictably and create continuous, explicitly connected road/lane sections, including widening and narrowing in both travel directions.

**Entry:** Phase 1 implementation and evidence explicitly approved, with no unresolved prerequisite blocker. Earlier suites remain regression requirements.

**Editor coverage:** World mesh settings and Road & lanes become functional; a point reads its before/after profiles from connected sections. Legacy detail references: 13–24.

**Requirements:** R02 · R04 · R05 · R06 · R07 · R08 · R09 · R10 · R17. Definitions are in [Master requirements](../01-MASTER-REQUIREMENTS.md).

## Deliverables

- Per-entry Highway → City → State → Root mesh resolution with provenance and actionable failures.
- Stable lane profiles, compatible mesh interfaces and explicit 6→4→2 directional mappings.
- Road/adapter preview artifacts and lane-corridor data from the same staged snapshot, with safe replacement.

## Twenty short tasks

1. Create reusable versioned mesh catalogs with semantic roles, variants and component-slot metadata.
2. Resolve each slot Highway → City → State → Root; points and places select roles, not new cascade levels.
3. Differentiate unassigned entries from broken explicit assets; inherit only when unassigned and block broken requirements.
4. Show effective value, inherited source, local assignment, clear-override action and descendant impact.
5. Implement explicit shared-topology mesh-owner resolution without relying on rebuild order.
6. Validate mesh units, orientation, pivot, port frames, widths, materials and profile compatibility.
7. Define stable lane IDs, independent directions, widths, access classes, speed defaults and driving-side policy.
8. Use highway-default plus segment-section profiles; derive point before/after labels from those records.
9. Implement 6→4 as explicit forward 3→2 merge and reverse 2→3 branch connections.
10. Implement 4→2, approved 6→2 staging, width-only changes, one-way and asymmetric profiles.
11. Store transition station intervals, taper geometry and directional mapping without fractional lane identities.
12. Offer preset/automatic mapping proposals, but require visible decisions and block unresolved lane links.
13. Build continuous lane centers/corridors and verify widths, grade, banking, turn constraints and boundaries.
14. Preview road, sidewalk, curb, border and texture-based markings with compatible modular interfaces.
15. Generate render previews and lane data from one staged snapshot; mark unfinished outputs non-publishable.
16. Track generated output ownership and revisions; canceled/repeated builds must not touch unrelated assets.
17. Run cascade, compatibility, directional-adapter, mesh-seam and geometric-containment tests.
18. Test profile/catalog edits through undo/redo, source rescan, save/reopen and failure recovery.
19. Collect resolved-catalog fixtures, lane mappings, mesh-boundary evidence and data-preservation results.
20. STOP: present Phase 2 evidence and wait for explicit approval before starting Phase 3.

## Test cases — all NOT_EXECUTED

Each case below is a scenario that may contain multiple assertions. Passing a scenario requires all its listed outcomes and applicable failure variants. The case IDs are retained in the [test matrix](../verification/TEST-MATRIX.md); no result is inferred from task completion.

| ID | Scenario | Procedure | Expected result | Requirements |
|---|---|---|---|---|
| P02-T01 | Four-level precedence | Assign one slot at Root, State, City and Highway; clear them from nearest to farthest. | Source changes Highway → City → State → Root; clearing Root yields a typed missing-required-entry issue. | R05 |
| P02-T02 | Partial catalog and nulls | Override only a City shelter; leave its road entry absent; remove City/State config references. | Each slot resolves independently; absent asset/entry inherits and does not copy parent values into children. | R05 |
| P02-T03 | Broken explicit asset | Assign an invalid Highway path while a valid Root fallback exists; disable an optional feature separately. | Broken assignment blocks instead of silently falling through; disabled optional feature is not treated as null inheritance. | R05, R09 |
| P02-T04 | Mesh interfaces and shared owner | Resolve a shared-topology fixture using an explicit owner; swap an incompatible lane-width/port/pivot variant. | Owner choice is deterministic; mismatched interface produces an actionable error naming the port/slot. | R06, R10 |
| P02-T05 | 6→4 forward merge | Build the 3-forward-lane to 2-forward-lane fixture and remove the dropped lane successor. | All valid inputs have continuous approved successors; the negative fixture is blocked with lane identity. | R07, R08 |
| P02-T06 | 6→4 reverse branch | Traverse the same physical adapter in reverse travel, from two lanes toward three. | Explicit branch options exist; direction, access and no-wrong-way rules are preserved. | R07, R08 |
| P02-T07 | 4→2 and 6→2 staging | Build both travel directions for 4→2 and an explicitly supported direct/staged 6→2 preset. | Every lane transition is mapped and has enough validated geometry; unsupported abrupt transitions cannot publish. | R08 |
| P02-T08 | Asymmetric and one-way | Use 4+2, 2+0 and bus-only lanes with independent width/access settings. | Total count never substitutes for direction/profile semantics; forbidden participant types are rejected. | R07 |
| P02-T09 | Width/count edits and identity | Change lane width without count, then add/remove a lane and inspect stable ID remapping. | Widths and corridors refresh together; unaffected IDs survive and removed lane references are reviewed. | R07, R17 |
| P02-T10 | Interior adapter interval | Place a semantic adapter inside a curved source segment; edit its station range and rescan. | Range is explicit and valid; no hidden source split or Landscape mutation is introduced. | R02, R08 |
| P02-T11 | Driving-side convention | Mirror a controlled profile fixture between left/right driving using the approved policy. | Lane identity rules stay stable; directional ordering and allowed mappings change consistently. | R07, R08 |
| P02-T12 | Mesh seam and marking quality | Join straight, curve and adapter boundaries using existing assets. | Profile boundaries align; sidewalk/border continuity, normals and material conventions match; no unintended end caps or separate lane-marker meshes. | R06 |
| P02-T13 | Banked curve containment | Sweep the configured vehicle envelope through a banked curve and adapter using agreed tolerances. | Usable corridors and mesh surface agree; geometric failures are reported, not hidden by resampling or clamping. | R06, R08 |
| P02-T14 | Build failure and ownership | Repeat a preview build; inject failure/cancel before replacement with unrelated scene assets present. | No duplicate generated output; last valid preview remains; unrelated assets are untouched. | R04, R17 |
| P02-T15 | History, persistence and cache | Change a parent mesh and segment profile; undo/redo, rescan, save/reopen and compare resolved results. | Only affected derived output invalidates; authored values and inheritance semantics round-trip. | R04, R05, R07 |

## Verification options and evidence

| Method | Evidence required |
|---|---|
| Automated engine/build tests | Actual command, engine/toolchain and source revision, fixture/seed, full result log, observed outcome, failure details. |
| Manual editor workflow | Reproducible steps, expected versus observed behavior, selected stable IDs, screenshots/recording and target DPI/theme. |
| Persistence/data safety | Before/after source hashes, backup path, reopened fixture, intentional-difference report and fault/recovery results. |
| Independent review | Changed files, requirement/test coverage, unresolved defects and an explicit user/reviewer decision. |

Methods complement one another. A screenshot is not proof of routing, a compile is not proof of traffic behavior, and browser simulation is not engine evidence. No test runner or Unreal project is delivered by this documentation package. Implement scenario fixtures using the [fixture catalogue](../verification/REGRESSION-FIXTURES.md).

## Boundary and dependencies

No complete intersection authoring or production driving AI yet. Canonical mesh-owner contracts are defined here; Phase 3 supplies full shared-intersection payloads. No new point-level catalog cascade.

Use the same versioned record/snapshot contracts introduced in earlier phases; do not create temporary competing models. Fix a prerequisite regression before asking for gate approval. Real undo/redo, stable IDs and source-preservation rules apply to every new accepted mutation.

## Exit gate

All 20 tasks addressed; all 15 phase scenarios executed with evidence and passing, or any noncritical deviation explicitly reviewed before approval. Critical data loss, invalid driving routes, broken references or mixed-revision publication cannot be treated as acceptable polish. Record earlier-phase regressions as failures, not new scope.

Fill [Phase gate report](../verification/PHASE-GATE-REPORT-TEMPLATE.md), attach actual evidence and stop. Do not start Phase 3 without explicit approval of this phase.

Current gate: `Implementation = NOT_STARTED; automated engine tests = NOT_EXECUTED; manual engine verification = NOT_EXECUTED; reviewer = PENDING`.

[Roadmap](../02-FIVE-PHASE-ROADMAP.md) · [Decision register](../decisions/DECISION-REGISTER.md) · [Start here](../00-START-HERE.md)
