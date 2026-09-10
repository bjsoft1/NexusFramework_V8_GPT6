# Nexus Framework V8 — editor prototype review package

## Open the prototype

Extract the ZIP completely. Open `index.html` in a desktop browser, or double-click `OPEN-PROTOTYPE.bat` on Windows. Press F11 or the toolbar fullscreen button for the full landscape workspace. The primary design size is 1920 × 1080 at 100% browser zoom.

`NexusFramework_V8_Review.html` is the same interactive prototype in a single self-contained file. It contains all styles, scripts, icons, scene illustrations, and synthetic fixtures. No server, npm install, Unreal installation, internet connection, or API key is required for the prototype itself. Browser policy may restrict local HTML or fullscreen; use a normal trusted desktop browser and preserve your exported feedback.

For a no-JavaScript visual overview, open `ALL-SCREENS.html`. Every screenshot opens at full 1920 × 1080 resolution.

## Contents

- 60 named entry HTML pages in `pages/`, plus a shared `index.html` application.
- Five workflow groups, with 12 screens each.
- Original CSS, JavaScript, inline SVG icons, and schematic isometric scene illustrations in `assets/`.
- Dark/light themes, screen search, command palette, Previous/Next navigation, local field drafts, review feedback, and fixture downloads.
- Interactive per-entry mesh fallback and independent schematic gizmo-offset states.
- Five **provisional** phase Markdown files, each with exactly 20 numbered points and eight concrete future test cases.
- A 20-point review summary, complete screen catalog, source traceability, decision register, and runtime-contract explanation.
- Browser/static test scripts, execution evidence, and generated screenshots/contact sheets.

## What works in the browser

Navigate the screens; search all 60 using Ctrl/Cmd+K; switch theme; inspect every field and acceptance note; change the four-level mesh assignments; edit/apply/revert sample fields; move/rotate/scale the schematic gizmo; select independent point states; inspect local mesh filenames; run labeled compilation/route demonstrations; export JSON fixtures or your screen-by-screen review notes.

Review notes, applied sample fields, theme, and resolver assignments use browser local storage when available. Browser storage can be restricted, cleared, or isolated by local-file origin. Export feedback before closing or switching entry documents. Reviews do not leave your machine unless you share the exported JSON.

## What is intentionally not implemented

This is not a UE project rebuild. It does not read a live Landscape, import FBX/GLB geometry, generate road meshes, compile Unreal data, execute driving AI, write your source project, or publish production artifacts. Viewports and mesh thumbnails are original vector schematics. The scene examples use independent illustrative scenarios; the downloadable runtime fixture is a smaller, internally referenced subset, not a compile of the scene.

Only marked browser interactions change. Most geometry and summary tables remain fixed design examples even when Details draft values change. Numeric validation is intentionally a small sample, not the future domain validator. There is no simulation of engine-local/world coordinate conversion or collision-safe transforms.

Undo/redo is deliberately disabled in the prototype, as requested. Real phases require Unreal-authoritative transactional undo/redo, including gizmo drags, hierarchy edits, catalog changes, and batch operations.

## Approval order

**Prototype approval → final five-phase plan approval → implement Phase 1 → test and review → explicit approval → Phase 2 … Phase 5.**

The files in `phases-draft/` are included only to make the proposed split reviewable. They are not a finalized schedule or permission to start. No implementation phase is complete in this package. A local “Mark reviewed” click is only prototype feedback.

## Recommended review route

Start at screen 01. Pay particular attention to screens 06 (source authority), 14–18 (mesh fallback), 20–23 (lanes and geometry), 30–38 (service payloads and gizmos), 44–48 (runtime graph contracts), 51 (schema), and 60 (approval). Use Request changes and export feedback at the end.

## Original project safety

The uploaded archive was inspected for reference only. Its source files were not overwritten, built, or modified. See `docs/SOURCE-TRACEABILITY.md` for the source archive SHA-256 and inspected paths.

## Verification

Read `evidence/TEST-REPORT.md` for actual performed checks and environment limitations. Real UE builds, persistence tests, runtime behavior, performance profiling, and migration tests are not executed by this prototype package.
