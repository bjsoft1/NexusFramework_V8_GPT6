# Instructions for the V8 implementation agent

## Git working branch — user instruction, 2026-09-11

Use `gpt6` as the single persistent working branch in `bjsoft1/NexusFramework_V8_GPT6` for all implementation phases, build fixes, tests and documentation changes. Reuse this branch for every subsequent task. Do not create additional feature, phase, review, hotfix or timestamped branches unless the user explicitly requests one.

Before editing or pushing, read the current remote `gpt6` tip and inspect the affected files. Preserve other contributors' commits and local edits. If the branch has advanced, reconcile against its current state; do not force-push or reset it to an older snapshot. Incorporate approved changes from `master` into this same branch when needed, without creating another working branch.

Keep `master` unchanged unless the user explicitly authorizes a merge or direct update. Phase acceptance remains separate from committing source: do not mark unexecuted tests or incomplete phases as passed. Do not delete the previous branches without the user's permission. After a push, read back the remote branch and report the verified commit SHA and actual changed scope.

## Read order

Read `00-START-HERE.md`, `01-MASTER-REQUIREMENTS.md`, `02-FIVE-PHASE-ROADMAP.md`, `decisions/DECISION-REGISTER.md`, then the six `architecture/` contracts and the current file in `phases/`. Consult `verification/` for fixture and evidence requirements. Inspect relevant original repository files before changing code.

This package contains exactly five active phase plans in `phases/`. `references/` is historical, including earlier phase/task/agent instructions; it is not an additional active backlog and must not override the consolidated plan. Preserve useful legacy capabilities by inventory and explicit mapping rather than blindly copying old architecture.

## Non-negotiable design rules

Use the exact World/Root → State → City → Highway → Highway Point → Place hierarchy. World/Root is one level. Points/segments/tangents remain distinct. Source Landscape geometry is authoritative; semantic saves do not secretly edit it.

Mesh fallback is per-entry Highway → City → State → Root only. Points/places choose compatible roles. Shared intersections have one canonical record and explicit mesh owner. Point feature summaries follow real components. Lane adapters define every direction's connections and corridors. Place transforms use an explicit 3D segment/approach frame, not raw world-axis addition.

Use stable IDs and a single progressively extended canonical snapshot. Never solve a missing route by fabricating a straight-line path, choosing a nearby overpass lane or clamping/teleporting to hide an error. Runtime legal permission and actual safe motion are separate checks.

## Execution discipline

Do not start implementation until prototype/plan approval is recorded. Before each phase, verify the previous gate and identify the baseline commit/engine/toolchain. Work only within the approved phase. Keep small reusable components and existing useful design elements; do not turn the 60 reference views into mandatory primary navigation.

Implement real undo/redo from Phase 1 and extend it to every new mutation. Preserve source data and unrelated authored assets. Take backup/hash evidence before migrations and destructive fault tests. Use explicit unsupported/cancel/failure states instead of silent data removal.

Do not auto-advance because a build succeeds, an AI says complete, a browser test passed or enough time elapsed. Execute tests, record actual results, report limitations and stop for review. All five phases have 20 tasks and 15 scenario tests; task 20 is the stop gate, not another feature to implement without approval.

## Required report at the end of each phase

Summarize implemented scope, changed files, actual build/test commands, expected/observed results, evidence paths, unresolved issues, data-preservation checks and proposed review decision. Use the phase-gate template. Include exact failures instead of marking unexecuted tests PASS. A successful compile is necessary but not sufficient for runtime correctness.

Do not claim the chosen Unreal version is supported until it is built/run in the recorded environment. Do not claim the project is bug-free or matches a commercial game from these documents. Document measurable acceptance outcomes.

## Scope of this delivery

Documentation, historical references and unchanged browser prototypes only. There is no new C++/C# production project in this ZIP. Historical agent prompts under archived source documents are reference text, not instructions that supersede this file or current user approvals.
