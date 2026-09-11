# V8 master requirements — twenty review points

Revision: V8.1-aligned five-phase documentation / 1.0 · 2026-09-10. This is the current consolidated requirement baseline for review, not proof of implementation.

The goal is a powerful dynamic road-network editor with a simple everyday UI: mesh generation and runtime navigation use the same validated information. Visual/simulation ambition is a product direction, not a guaranteed match to a commercial game.

| ID | Requirement | Required behavior |
|---|---|---|
| R01 | **Exact hierarchy and stable identity** | World/Root is one level; State → City → Highway → Highway Point → Place. Stable GUID identity is independent of names, arrays, actors and streaming cells. |
| R02 | **Landscape authority and source reconciliation** | Points and segments remain distinct. Each segment end has its own tangent/frame. Read source geometry, preserve authored semantics and explicitly reconcile changed or unavailable sources. |
| R03 | **Simple, powerful editor** | Six contextual workspaces, one central viewport, hierarchy and contextual inspector. Compact labeled icons, dark/light themes, keyboard access, readable 1080p/DPI behavior and optional Advanced. |
| R04 | **Undo/redo, persistence and no silent data loss** | Real transactions, explicit draft/apply/revert, atomic save and publish, recoverable migration, backups and failure tests. No unreviewed deletion or hidden source write. |
| R05 | **Four-level per-entry mesh fallback** | Highway → City → State → Root, independently per role/variant slot. Points/places do not add a config scope. Missing inherits; broken explicit assignment is diagnosed. |
| R06 | **Mesh/profile/port compatibility** | Existing highway, adapter, junction and roundabout meshes must agree with lane widths, boundary geometry, orientation, pivots, units, ports and material conventions. |
| R07 | **Explicit lane profiles** | Stable lane identity, independent directions and widths, sidewalk/border/median data, access and speed rules. Point before/after displays are derived, not duplicate lane-count fields. |
| R08 | **Directional adapters** | 6→4→2 and supported 6→2 use explicit mappings, station extents, taper geometry and priority. A symmetric 6→4 physical change is a forward merge and reverse branch. |
| R09 | **Typed point features** | Crosswalk, signal, adapter and intersection summaries reflect linked versioned components. Roundabout is a junction subtype. Contradictory flags and unknown runtime handlers block publication. |
| R10 | **Canonical shared junctions** | One intersection record, explicit per-approach frames/lane profiles, legal turn connectors, control/conflict rules and deterministic mesh-owner policy. |
| R11 | **Canonical roundabouts and query semantics** | Unique IDs, ring lanes, circulation, entry/exit rules and reachable exits. Count canonical objects, not approach references; legal exits are not physical arm counts. |
| R12 | **Crosswalks and signals** | Full road-edge crossing span and separate sidewalk-plus-border depth rule; waiting points, walk links, controlled movements, timing and conflict-safe phase definitions. |
| R13 | **Point-attached places** | Parking, bus stop, hospital, city park and petrol pump with explicit parent point, frame-relative transforms and legal vehicle/pedestrian/service access. |
| R14 | **Independent 3D gizmos** | Full local-frame composition, explicit junction approach, independent N≥3 sub-points, stable handle identity, snapping, constraints, cancel, batch and preserve-world/local reparent. |
| R15 | **Complete service semantics** | Bus queue/board/alight/shelter/base/indicator, physical/logical seats, parking bays/dimensions/gaps, hospital/park/fuel access; mutable reservations and occupancy are separate. |
| R16 | **Legal graph and query contracts** | Successors, permitted lane changes, junction/roundabout movements, upcoming features and reachable service entrances; typed failure and elevation-aware bounded lane projection. |
| R17 | **Coherent compiler output** | One validated versioned snapshot feeds road meshes, lane/walk graphs, service records and manifests; deterministic hashes and atomic publication prevent mixed revisions. |
| R18 | **Actual runtime safety behavior** | Controllers check gaps, conflicts, braking, geometry and live state. Verify lane following, merges, turns, crossings and stop/wait/replan; no fabricated routes or concealed teleport recovery. |
| R19 | **Extensibility and large worlds** | Versioned type registry with validated handlers; deterministic placement, logical/visual separation, graph availability, runtime-state persistence, pooling and measured budgets. |
| R20 | **Evidence and sequential approval** | Five phases, reviewed prototype/plan, explicit per-phase approval, reproducible commands and expected/observed results. Package checks, browser tests and engine tests are different evidence classes. |

## Non-negotiable relationships

```text
Ownership:   World/Root > State > City > Highway > Highway Point > Place
Mesh lookup: Highway > City > State > Root
Source:      Landscape geometry + separate Nexus semantic authoring
Output:      One versioned snapshot > compatible mesh + lane/walk/service data
Progress:    Approve prototype/plan > implement/test/review one phase > explicit approval
```

## Current versus historical material

Current phase files are only in `phases/`. Current detailed contracts are in `architecture/`. Prior V8 and V8.1 documents are retained unchanged in `references/`, including earlier drafts and historical browser evidence. Legacy V0–V7 documentation is in a separate reference ZIP. Archive content must not override this requirement baseline or current approvals.

See [Requirement coverage](verification/REQUIREMENT-COVERAGE.md), [Roadmap](02-FIVE-PHASE-ROADMAP.md) and [Decision register](decisions/DECISION-REGISTER.md).
