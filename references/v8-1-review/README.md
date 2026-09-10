# NexusFramework V8.1 — simplified editor review

Open `index.html`, or use the standalone `NexusFramework_V8_1_Simple_Review.html`. The HTML prototype needs no server, install, network, API key or Unreal project. All code and styles are local.

**Primary layout:** 1920 × 1080 landscape, browser zoom 100%. F11 enters browser fullscreen. Smaller desktop layouts are also included; this is not a mobile interface.

## What changed

Six contextual workspaces replace the original screen-list navigation: World, Road & lanes, Point features, Places, AI routes and Verify. The hierarchy now explicitly reads World/Root → State → City → Highway → Highway Point → Place. Advanced reveals deeper data without adding permanent panels.

Start with P-02 for the 6-to-4 adapter; P-04 shows 4-to-2. P-03 is the shared junction; P-05 is the roundabout. Select a child place to edit local offsets and drag its X/Y/rotation handles. The type picker adds parking, bus stops, hospitals, city parks and petrol pumps.

## Suggested review

1. Select P-02. Inspect the ending lane's successor, then select P-04.
2. Select P-03 and P-05. Review types, features, shared IDs and roundabout exit concepts.
3. Select Central bus stop. Change offsets or drag a handle; reparent to a different point and observe the labelled preserve-local behavior.
4. Open AI routes. Query F3 to the east exit, then choose “Try a missing lane link”. The query should return NO PATH.
5. Open World. Assign/clear catalog levels and inspect the effective highway source.
6. Open Verify and export your notes/results with Export review before closing the tab.

## What actually works

In-memory navigation and selection; themes; Advanced disclosure; independent place transforms and basic creation/reparenting; crosswalk/signal component existence switches; positive-number input constraints; one sample mesh-entry cascade; finite forward-graph BFS; typed participant/closed-exit filtering; local fixture checks; review JSON export.

## Boundaries

The viewport is a schematic, not Unreal geometry. Lane-width/taper edits update review inputs but do not rebuild the reference road shape. The graph covers a small forward-only example; it is not a complete junction/roundabout/service traffic network or a driving controller. It does not simulate lane changes, collision avoidance, 3D banked frames, pedestrians, actual asset compatibility, Landscape access or compilation. Alternate topology templates deliberately fail sample-graph compatibility checks.

Only the default bus-stop pose has a sample service route. Moving/reparenting/rotating that stop invalidates its fixture connector; a production compiler would rebuild and validate it. Other service access IDs are illustrative placeholders, not validated full routes.

Edits are memory-only. Export captures state and feedback; importing/restoring that JSON is not implemented. Reload starts a fresh sample. Real undo/redo, durable saves and transactional recovery belong in implementation. No production runtime export is offered.

## Contents

- `index.html`, `assets/`: revised six-workspace prototype source.
- `NexusFramework_V8_1_Simple_Review.html`: same prototype in one file.
- `docs/20-POINT-REVISION.md`: short requirement checklist.
- `docs/ROAD-NETWORK-CONTRACT.md`: proposed model and runtime contract.
- `docs/ENGINE-ACCEPTANCE-TESTS.md`: 36 future engine acceptance cases, all unexecuted.
- `advanced-reference/`: original 60-screen HTML/JS/CSS reference, not all reworked; its previous architecture text can be superseded by this revision.
- `prior-phase-drafts/`: five original draft phase files, unchanged and unapproved.
- `tests/`, `evidence/`: local browser test source and actual results/screenshots.

The source upload and previous prototype package remain unchanged. This revision does not approve or start an implementation phase.
