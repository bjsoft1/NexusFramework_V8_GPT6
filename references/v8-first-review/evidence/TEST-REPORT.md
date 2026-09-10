# V8 prototype — actual verification report

Executed: 2026-09-10. Scope: HTML/CSS/JavaScript design prototype and synthetic data fixtures. No implementation phase was executed.

## Final results

| Verification | Executed | Passed | Failed |
|---|---:|---:|---:|
| 60 screens × dark/light at 1920 × 1080 | 120 | 120 | 0 |
| 12 representative screens at 1600 × 900 and 1366 × 768 | 24 | 24 | 0 |
| Browser interaction scenarios | 32 | 32 | 0 |
| Static package / syntax / data-reference assertions | 45 | 45 | 0 |

Total: **221 final assertions/scenarios passed** across the categories above. No uncaught browser JavaScript errors occurred in the final render or interaction runs. Passing this scope is not a guarantee of bug-free behavior outside the tested cases.

## Browser environment and method

Chromium 144.0.7559.96, Linux, headless, device scale factor 1. The delivered standalone HTML was rendered with Playwright `page.set_content()`. File-URL and localhost HTTP navigation were blocked by this environment's browser policy; they were **not** treated as passing launch tests. The complete inline CSS/JS/fixture artifact was exercised, not a rewritten or mock application.

All 60 modular HTML entry documents were checked statically for correct initial screen IDs and existing local assets. Their actual direct `file://` loading, Windows `.bat`/PowerShell launching, native fullscreen behavior, Edge, Firefox, Safari, Windows display scaling, and browser-origin storage persistence remain unverified here.

The `about:blank` test context does not provide persistent local storage. Storage failure is caught and a user notice is shown. In-session applied fields and review data survived screen navigation; exported feedback preserved the notes. **Cross-restart persistence was not verified.** Export review feedback before closing. Transient storage-warning toasts were cleared before pristine screenshots; the warning handling was tested separately.

## What the checks covered

Render: expected title, route identity, theme, twelve workflow links per group, top-level scroll bounds, principal panel bounds, missing images, and intentionally disabled undo/redo. Representative responsive checks verify top-level fit, not exhaustive accessibility certification. Main content and side panels may intentionally scroll.

Interactions: navigation, command palette/empty states, cross-group search, themes, all four fallback levels, clearing overrides, missing required slots, per-entry resolution, null/blank transparency, parent updates, apply/revert, negative and fractional field validation, one-way and zero-total lane cases, feedback, downloadable JSON, schema tabs, catalog filtering, filename-only mesh intake, three independent sub-point states, pointer move/rotate/scale, Escape cancellation, compile simulation success/cancel/failure, fixture play/pause, bad-route recovery, modal focus, and unavailable-storage handling.

Static: sixty physical HTML files and unique screen records, guide/field coverage, offline dependency checks, inlined-source consistency, five draft files with twenty numbered points and eight future tests each, four JavaScript syntax checks, GUID/parent/graph references, lane-change interval bounds, signal and pedestrian references, catalog slots, complete bus/seat/parking payload fields, parking-specific serialization, shared seat-pose consistency, separate dynamic state, crossing-depth rule, runtime JSON round-trip, and mesh-manifest reference ownership. Null highway config references correctly inherit; they are not dangling IDs.

## Visual evidence

`evidence/screens/`: 60 dark full-resolution screenshots plus six light examples (18, 20, 35, 51, 58, 60). `evidence/contact-sheets/`: five twelve-screen overviews. `ALL-SCREENS.html`: static no-JavaScript gallery. `evidence/PREVIEW.jpg`: four representative screens. Contact sheets summarize the screenshot captures; they are not additional UI screens.

## Explicitly not verified or implemented

No Unreal Editor/UHT build, C++ compilation, Landscape read/write, real FBX/GLB importer, native docking, native world/local transform conversion, undo/redo transaction stack, actual road generation, mesh seam correctness, driving physics, signal conflict solver, AI route execution, occupancy concurrency, migration of a real project, save-crash recovery, packaged runtime launch, performance benchmark, or zero-data-loss guarantee was performed.

The runtime fixture is a smaller illustrative data subset with tested references. Larger viewport counts and simulated metrics are independent UI scenarios, not claims about a compiled city. Lane graph correctness and collision safety still require real domain/engine tests.

## Input safety

Original archive SHA-256 remained `aa0363d9936f94bb75b7abd5b66f140dd1b4c415f0f49a61e12683e429d428ac` after generation. The uploaded archive was inspected but not overwritten, extracted over an existing project, compiled, or executed. Only newly generated prototype files were created.

## Reproduction

Run `python tests/static_checks.py` for standard-library checks (Node adds JS syntax validation when available). For browser tests install Playwright and its Chromium binary, or provide an installed Chromium path; then run `python tests/browser_smoke.py --screenshots` and `python tests/interaction_smoke.py`. See `tests/README.md`.

Raw evidence: `browser-render-01-60.json`, `interaction-results.json`, `static-results.json`. The five real implementation phase test suites are explicitly **unexecuted** and remain behind approval gates.
