# Phase 1 — Foundation, editor shell & Landscape authority

**Status: PROVISIONAL DRAFT — NOT APPROVED — NOT STARTED.**

These are 20 review points, not authorization to start implementation. First approve the 60-screen prototype, then finalize all five phase plans. Real implementation runs sequentially and stops at each gate. Existing V0–V7 phase numbers do not carry forward into this new five-phase plan.

**Prototype coverage:** 01–12.
**Entry condition:** Prototype and five-phase plan explicitly approved.

## Twenty short implementation and verification points

1. Confirm the approved prototype, data boundaries, and supported Unreal build before coding.
2. Create isolated runtime, authoring, and editor modules with no runtime dependency on editor code.
3. Build independent Hierarchy, Details, and Activity tabs; keep them available outside Landscape Mode.
4. Implement the compact toolbar, command palette, dark/light styling, focus states, and keyboard navigation.
5. Create versioned World → State → City → Highway records with stable GUID identity and developer codes.
6. Centralize selection, commands, validation, draft state, and operation feedback across all UI entry points.
7. Implement draft → diff → impact → Apply; commit each accepted action through one Unreal transaction.
8. Implement transactional create, rename, reparent, and delete with explicit dependency previews.
9. Implement atomic persistence, revision-aware recovery, and separate draft storage without overwriting sources.
10. Read multiple Landscape sources and unloaded partition states without mutating terrain or spline geometry.
11. Implement Scan → Diff → Review → Apply bindings; preserve authored semantic fields during rescan.
12. Prove one highway vertical slice: scan points/segments, bind ownership, inspect geometry, serialize a minimal snapshot.
13. Test clean editor build, UHT, module dependency boundaries, tab lifecycle, and shutdown without errors.
14. Test first-run create/edit/apply/revert plus committed undo/redo from toolbar, menu, and shortcuts.
15. Test rename and reparent preserve IDs, reject cycles, and reconcile selection after undo/redo.
16. Test save/restart, interrupted save, stale draft conflicts, duplicate imports, and future-schema rejection.
17. Test source fingerprints before/after scan/save; Landscape content must remain unchanged.
18. Test curved tangents, closed loops, shared junction points, multi-Landscape ownership, and unloaded cells.
19. Collect browser-reference comparisons, engine screenshots, automation logs, schema fixtures, and source hashes.
20. STOP: review Phase 1 evidence and obtain explicit approval before implementing Phase 2.

## Required test cases — all UNEXECUTED

| ID | Test | Procedure | Expected result |
|---|---|---|---|
| P1-T01 | Clean build and module scan | Build the chosen Editor target; inspect runtime dependencies. | Build succeeds; no editor dependency leaks into runtime. |
| P1-T02 | Transactional authoring | Create, rename, reparent; undo/redo each action. | One named undo record per accepted mutation; GUIDs remain stable. |
| P1-T03 | Draft vs committed state | Apply A; draft B; Revert; Undo A. | Revert removes only B; engine Undo reverses A. |
| P1-T04 | Authority safety | Hash Landscape assets; scan, reconcile semantic bindings, save. | No geometry write or terrain mutation from read/save paths. |
| P1-T05 | Save and recovery | Interrupt a save; reopen with a stale draft revision. | Last valid source survives; base/current/draft comparison is required. |
| P1-T06 | Spline edge cases | Load curved, negative-tangent, loop, junction, and unloaded-cell fixtures. | Correct bindings/tangents; unloaded does not mean deleted. |
| P1-T07 | Vertical slice | Scan and serialize one highway, reopen its schema fixture. | Point and segment identities, links, and geometry round-trip. |
| P1-T08 | UI usability | Operate core workflow keyboard-only at 1080p and 125% DPI. | No blocked primary action, lost focus, or unreadable essential field. |

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
