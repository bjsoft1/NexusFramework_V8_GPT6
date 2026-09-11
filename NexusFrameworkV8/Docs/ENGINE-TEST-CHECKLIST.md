# Phase 1 and 2 engine acceptance checklist

**All native checks remain NOT_EXECUTED in this environment.** Some acceptance scenarios require implementation work, not just a rerun. Portable results cannot close these gates.

## Complete scenario definitions

The unchanged plans contain all thirty scenario procedures and expected results:

- [Phase 1: P01-T01 through P01-T15](Plans/phases/01-Foundation-and-Landscape.md)
- [Phase 2: P02-T01 through P02-T15](Plans/phases/02-Mesh-Inheritance-Roads-and-Adapters.md)
- [Regression fixtures](Plans/verification/REGRESSION-FIXTURES.md)
- [Phase approval report template](Plans/verification/PHASE-GATE-REPORT-TEMPLATE.md)

## First local run

Close Unreal instances using this project. Build with `BUILD-EDITOR.bat`, or pass your engine directory to `Scripts/Build-Editor.ps1`. Resolve any build failure before describing the editor as working.

Open Tools > Nexus V8 Road Editor. Use Demo > Build preview for the synthetic two-segment 6-to-4-to-2 fixture. Inspect lane directions, profile edits, missing-successor rejection, Apply/Revert, Undo/Redo, save/reopen, and catalog fallback. Generated preview must stay explicitly non-publishable.

On a separate backed-up test map, inspect a real Landscape with three connected segment ends and unequal signed tangents, a loop, a banked curve, an overpass, source split/rename and unloaded-partition cases. Scan/review/apply must not secretly mutate Landscape geometry. Compare before/after source hashes and authored values.

Test strict meshes using real compatible and incompatible lane-width/port/pivot assets. A declared interface is not proof of a matching physical mesh. Check seams, materials and vehicle swept envelopes in the real editor.

`RUN-UNREAL-TESTS.bat` runs the eight supplied `NexusV8.Automated` logic/transaction tests under NullRHI. It does not verify rendering, viewport input, real asset seams or all thirty scenarios. Record those separately.

## Result record

```text
Scenario ID:
Engine / toolchain / commit:
Fixture and input revision:
Command or reproducible editor steps:
Expected:
Observed:
Result: PASS / FAIL / BLOCKED / NOT_EXECUTED
Evidence paths:
Remaining implementation or fixes:
Reviewer decision:
```

Do not mark an incomplete scenario PASS because the portable suite succeeds. Do not advance an acceptance gate automatically.
