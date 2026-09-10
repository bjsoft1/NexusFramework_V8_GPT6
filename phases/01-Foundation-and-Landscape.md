# Phase 1 — Foundation, simple editor and Landscape authority

**Plan:** consolidated for review. **Implementation:** NOT_STARTED. **Engine tests:** NOT_EXECUTED. **Approval:** PENDING.

Revision: V8.1-aligned five-phase documentation / 1.0 · 2026-09-10

## Goal and entry gate

Establish a safe, usable authoring foundation with the exact hierarchy, stable source bindings and real transactions before adding road-generation breadth.

**Entry:** Prototype and five-phase plan explicitly approved; baseline backup and environment recorded.

**Editor coverage:** World is functional. The other five workspaces exist as contextual shells, without implying that their future tools work. Legacy detail references: 01–12 and 58.

**Requirements:** R01 · R02 · R03 · R04 · R13 · R17 · R19 · R20. Definitions are in [Master requirements](../01-MASTER-REQUIREMENTS.md).

## Deliverables

- An editor/runtime module boundary and recorded target-engine/toolchain baseline.
- World/Root → State → City → Highway → Highway Point → Place identities, with a minimal versioned snapshot.
- Read-only Landscape scan, diff, reconciliation, save/reopen, recovery and real undo/redo.

## Twenty short tasks

1. Record the approved prototype/plan, target engine, toolchain, repository baseline and clean-worktree backup.
2. Inventory V0–V7 features and regressions; map each to keep, replace, defer or reject with reasons.
3. Create separate runtime, authoring/compiler and editor modules without runtime dependencies on editor code.
4. Build the six-workspace shell with compact labeled icons, Basic/Advanced disclosure and dark/light themes.
5. Implement World/Root → State → City → Highway → Highway Point → Place ownership using stable GUIDs.
6. Keep segment links and shared source/topology references separate from the ownership tree.
7. Read Landscape control-point/segment geometry through source bindings; never cache competing authored coordinates.
8. Expose every connected segment end and its independent tangent, direction, transform and source identity.
9. Track source identity and load state across maps/partitions; distinguish unloaded, missing and deleted records.
10. Implement Scan → Diff → Review → Apply, preserving metadata and requiring remapping after splits/merges.
11. Introduce versioned feature/place type registration and preserve unsupported payloads without executing them.
12. Define canonical shared-topology IDs and owner/reference contracts for later junction and roundabout payloads.
13. Centralize commands and accepted mutations into real transactions with Apply, Revert, Cancel, Undo and Redo.
14. Invalidate derived previews on source or authoring changes without silently overwriting saved semantics.
15. Implement staged atomic saves, independent drafts, revision conflicts, backups and recoverable schema migration.
16. Serialize one minimal immutable snapshot through the compiler boundary that later phases will extend.
17. Run build, hierarchy, source-binding, tab lifecycle, keyboard and DPI checks on real editor fixtures.
18. Exercise undo/redo, save/reopen, interrupted writes and source-hash preservation with recorded failures.
19. Collect source diffs, engine screenshots, commands, logs and requirement coverage for independent review.
20. STOP: present Phase 1 evidence and wait for explicit approval before starting Phase 2.

## Test cases — all NOT_EXECUTED

Each case below is a scenario that may contain multiple assertions. Passing a scenario requires all its listed outcomes and applicable failure variants. The case IDs are retained in the [test matrix](../verification/TEST-MATRIX.md); no result is inferred from task completion.

| ID | Scenario | Procedure | Expected result | Requirements |
|---|---|---|---|---|
| P01-T01 | Build and module boundary | Configure the recorded engine/toolchain; build the Editor target and a runtime-only module/dependency check. | Both succeed with no runtime dependency on editor modules; archive commands and logs. | R03, R20 |
| P01-T02 | Exact ownership tree | Create each hierarchy level and all five place-type identity shells; try a cycle and orphan parent. | World and Root are aliases for one record; valid place parents are highway points; invalid ownership is rejected. | R01 |
| P01-T03 | Stable IDs and reparent | Rename and reparent a city/highway/place; undo/redo; save/reopen. | GUIDs and references remain stable; dependency review identifies affected descendants. | R01, R04 |
| P01-T04 | Per-segment-end tangents | Use a point with at least three connected segment ends and unequal/negative tangent values. | Each endpoint retains its own binding and values; no single shared tangent replaces them. | R02 |
| P01-T05 | Closed loops and duplicates | Scan a closed loop, repeated shared point and repeated import; rescan twice. | No duplicate points/segments or broken loop closure; source order does not redefine identity. | R02 |
| P01-T06 | Source transforms and grade separation | Read rotated/scaled source actors and roads crossing at different elevations. | Resolved geometry is consistent in the declared units/frame; XY overlap does not create topology. | R02 |
| P01-T07 | Rescan preserves semantics | Attach authored codes, feature stubs and place offsets; move source geometry; reconcile. | Compatible authored values survive and derived transforms update without authoring copies of source geometry. | R02, R13 |
| P01-T08 | Split/merge reconciliation | Split and merge bound source segments; attempt Apply with unmapped references. | Affected IDs are remapped explicitly or blocked; no orphan places or silently discarded metadata. | R02, R04 |
| P01-T09 | Unloaded is not deleted | Unload a source partition, scan and reopen it. | State is unavailable/unloaded until resolved; no deletion is inferred from temporary absence. | R02, R19 |
| P01-T10 | Map and tab lifecycle | Change maps, reopen tabs, switch mode and alter selection during source refresh. | Old source handles are invalidated; no stale callbacks, stuck selection or unintended source edit occurs. | R02, R03 |
| P01-T11 | Transactional edits | Create/apply A; draft B; cancel/revert B; undo/redo A from toolbar and keyboard. | One accepted action is one transaction; cancel restores pre-drag state and Revert affects only the draft. | R04 |
| P01-T12 | Interrupted save and conflicts | Inject failure before replacement and open a stale draft against a newer saved revision. | Last valid save survives; recovery and three-way conflict review are explicit; no data is overwritten silently. | R04 |
| P01-T13 | Landscape preservation | Fingerprint Landscape content before and after scan, semantic save and reopen. | Read/save paths leave geometry assets unchanged; no rebuild or terrain mutation is hidden in saving. | R02, R04 |
| P01-T14 | Schema and legacy import | Load a supported test legacy fixture, unknown schema version and duplicate import into a backup workspace. | Supported data has a reviewed diff; unsupported data is preserved/rejected safely; import is repeatable. | R04, R19, R20 |
| P01-T15 | Vertical slice and usability | Author one highway and place; serialize/reopen; operate Basic at 1920×1080 and 125%/150% display scale. | IDs/bindings round-trip; dark/light, focus, tooltips and ordinary authoring work without opening Advanced. | R01, R03, R17 |

## Verification options and evidence

| Method | Evidence required |
|---|---|
| Automated engine/build tests | Actual command, engine/toolchain and source revision, fixture/seed, full result log, observed outcome, failure details. |
| Manual editor workflow | Reproducible steps, expected versus observed behavior, selected stable IDs, screenshots/recording and target DPI/theme. |
| Persistence/data safety | Before/after source hashes, backup path, reopened fixture, intentional-difference report and fault/recovery results. |
| Independent review | Changed files, requirement/test coverage, unresolved defects and an explicit user/reviewer decision. |

Methods complement one another. A screenshot is not proof of routing, a compile is not proof of traffic behavior, and browser simulation is not engine evidence. No test runner or Unreal project is delivered by this documentation package. Implement scenario fixtures using the [fixture catalogue](../verification/REGRESSION-FIXTURES.md).

## Boundary and dependencies

No production traffic controller, full road meshing, service-specific editor, final streaming system or unsupported automatic legacy migration. Creating the documentation is not authorization to start this phase.

Use the same versioned record/snapshot contracts introduced in earlier phases; do not create temporary competing models. Fix a prerequisite regression before asking for gate approval. Real undo/redo, stable IDs and source-preservation rules apply to every new accepted mutation.

## Exit gate

All 20 tasks addressed; all 15 phase scenarios executed with evidence and passing, or any noncritical deviation explicitly reviewed before approval. Critical data loss, invalid driving routes, broken references or mixed-revision publication cannot be treated as acceptable polish. Record earlier-phase regressions as failures, not new scope.

Fill [Phase gate report](../verification/PHASE-GATE-REPORT-TEMPLATE.md), attach actual evidence and stop. Do not start Phase 2 without explicit approval of this phase.

Current gate: `Implementation = NOT_STARTED; automated engine tests = NOT_EXECUTED; manual engine verification = NOT_EXECUTED; reviewer = PENDING`.

[Roadmap](../02-FIVE-PHASE-ROADMAP.md) · [Decision register](../decisions/DECISION-REGISTER.md) · [Start here](../00-START-HERE.md)
