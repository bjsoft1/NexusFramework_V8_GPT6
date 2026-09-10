# Phase 2 — Mesh cascade, lane profiles & road generation

**Status: PROVISIONAL DRAFT — NOT APPROVED — NOT STARTED.**

These are 20 review points, not authorization to start implementation. First approve the 60-screen prototype, then finalize all five phase plans. Real implementation runs sequentially and stops at each gate. Existing V0–V7 phase numbers do not carry forward into this new five-phase plan.

**Prototype coverage:** 13–24.
**Entry condition:** Phase 1 completed, evidence reviewed, and explicitly approved.

## Twenty short implementation and verification points

1. Confirm per-entry mesh fallback and semantic lane-profile ownership before freezing the schema.
2. Create reusable mesh-config data assets with extensible semantic slot keys and metadata.
3. Reference catalog assets only at Root, State, City, and Highway; exclude point-level mesh overrides.
4. Implement Highway → City → State → Root lookup; null assets and null entries remain transparent.
5. Show inherited source, local assignment, clear-override action, and affected descendants beside each entry.
6. Use asset pickers for existing project meshes; validate references, sockets, bounds, materials, and lane variants.
7. Author highway-default lane profiles and explicit per-segment profile references where lane counts change.
8. Preserve direction, width, speed, access classes, walking width, curb, and border semantics during source rescan.
9. Generate straight and curved road previews, sidewalks, curbs, markings, and 6 → 4 → 2 adapters.
10. Dimension crosswalk span separately from depth; use sidewalk width plus border width for the requested depth.
11. Generate from validated snapshots and resolved catalogs; never persist inherited effective copies into authoring.
12. Track source hashes and generator ownership for incremental previews and safe output promotion.
13. Test root/state/city/highway priority, null-asset transparency, partial entries, and clear-to-inherit behavior.
14. Test root edits invalidate only affected derived results; closer overrides remain unchanged.
15. Test unresolved and dangling required slots block builds without silently substituting engine defaults.
16. Test one-way roads, zero-width sidewalks, lane-count changes, and legal adapter mappings.
17. Test road/junction boundary profiles, normals, UV continuity, open modular joins, and texture-based markings.
18. Test repeat build, canceled build, failed build, undo/redo of catalog/profile changes, and save/restart.
19. Collect exact resolved-entry fixtures, generated-mesh comparisons, edge-case tests, and ownership manifests.
20. STOP: review Phase 2 evidence and obtain explicit approval before implementing Phase 3.

## Required test cases — all UNEXECUTED

| ID | Test | Procedure | Expected result |
|---|---|---|---|
| P2-T01 | Cascade precedence | Assign the same slot at all four levels; clear nearest assignments in sequence. | Effective source advances Highway → City → State → Root. |
| P2-T02 | Partial catalog | Override only City bus shelter; leave City road slot absent. | Shelter resolves from City; road still resolves from Root. |
| P2-T03 | Null asset | Remove City and State config references. | Resolution passes through to Root; no eager copies appear. |
| P2-T04 | Missing reference | Remove all required junction entries; use a dangling asset reference. | Typed blocking diagnostic; previous valid output remains. |
| P2-T05 | Profile semantics | Create one-way 2+0 lanes and a 6-to-4 segment profile change. | Valid one-way profile; adapter has explicit successor mapping. |
| P2-T06 | Modular seam quality | Join straight, curved, adapter, and upcoming junction boundary fixtures. | Positions, border widths, normals, and material conventions match. |
| P2-T07 | Rebuild ownership | Build twice, then cancel or inject an error during replacement. | No duplicate generated actors; original authored actors untouched. |
| P2-T08 | History and persistence | Edit config/profile; apply; undo/redo; save/reopen; rescan Landscape. | Assignments and semantics survive; resolved caches invalidate correctly. |

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
