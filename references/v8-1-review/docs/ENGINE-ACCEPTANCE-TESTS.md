# Required engine verification — NOT EXECUTED

These are future acceptance tests, not browser-test results. Each requires an executed command or reproducible editor procedure, build/schema version, exact fixture IDs, expected/observed result and evidence artifact. A reviewer must approve each implementation phase before the next starts.

| ID | Scenario | Required verification |
|---|---|---|
| ENG-01 | Exact ownership tree | World/Root is one level; services have point parents; no ownership cycles or orphan IDs. |
| ENG-02 | Point versus segment | A shared point with three or more connected segment ends retains each independent tangent/binding. |
| ENG-03 | Landscape reconciliation | Moving/splitting/rescanning source geometry preserves compatible feature/service semantics and flags unresolved mappings. |
| ENG-04 | 6 → 4 forward | All three incoming lanes have approved continuous successor corridors; the dropped lane merges legally. |
| ENG-05 | 6 → 4 reverse | Both incoming reverse lanes have explicit legal links to three outgoing lanes; no accidental wrong-way path. |
| ENG-06 | 4 → 2 and 6 → 2 | Validate both directions, lane identities, actual taper geometry and supported merge staging. |
| ENG-07 | Asymmetric/one-way profiles | 4+2, 2+0 and bus-only examples preserve direction, width and participant restrictions. |
| ENG-08 | Lane-width and count edits | Regenerate mesh and graph together; no stale old-width corridor or unmapped lane. |
| ENG-09 | Left/right driving convention | Lane numbering stays stable; circulation, allowed movements and directional geometry change consistently. |
| ENG-10 | Legal lane changes | Adjacent but prohibited lane is not returned as reachable; valid intervals honor markings and participant rules. |
| ENG-11 | Runtime lane-change safety | Reject currently occupied/unsafe gaps even when the static graph permits changing lanes. |
| ENG-12 | Junction turns | T/four-way/skewed/mixed-profile ports have correct legal turns; prohibited/U-turn cases remain rejected. |
| ENG-13 | Shared junction identity | Multiple highway references update one junction; duplicate geometry/connectors/controllers are not generated. |
| ENG-14 | Roundabout identity/count | One shared roundabout with multiple approach references counts once at city/world scope. |
| ENG-15 | Roundabout exit query | Entry-relative exit ordering, circulation, lane restrictions, class restrictions and U-turn policy match fixtures. |
| ENG-16 | Roundabout travel | Test single/multiple circulating lanes, entry yielding, missed-exit recovery and blocked exits without illegal cuts. |
| ENG-17 | Crosswalk semantics | Checkbox reflects real component; full span/depth geometry, waiting points, pedestrian links and conflicts are valid. |
| ENG-18 | Signal safety | No conflicting movements receive permission under the same valid phase; missing/stale control data blocks publish. |
| ENG-19 | 3D offsets | Translate/rotate/bank source road; each service follows its designated segment frame without offset drift. |
| ENG-20 | Independent sub-point gizmos | Edit three or more bus waiting/seat points independently; sibling transforms stay unchanged. |
| ENG-21 | Reparent behavior | Preserve-world and preserve-local modes produce documented results; undo/redo restores IDs, transforms and links. |
| ENG-22 | Parking | Route into/out of each bay, respecting bay size/vehicle class; reservation and occupancy remain separate runtime state. |
| ENG-23 | Bus stop | Bus entry/stop/exit, passenger waiting/boarding/alighting and seat links are independently navigable. |
| ENG-24 | Hospital/park/fuel services | Vehicles route to legal entrances and pedestrians to walk entrances, never blindly to mesh centers. |
| ENG-25 | Mesh fallback | Per-entry Highway→City→State→Root lookup, clear-to-inherit and missing/dangling asset errors work correctly. |
| ENG-26 | Mesh interface compatibility | Reject lane count/width, port, orientation or pivot incompatibility, including junction-owned catalog resolution. |
| ENG-27 | Coherent build output | Input change, cancellation or failure never publishes a new mesh with an old graph; keep last valid output. |
| ENG-28 | Geometric containment | Sample/sweep vehicle footprint through every curve, taper and connector; validate boundaries and turn radius. |
| ENG-29 | Grade separation | Overpasses/underpasses at the same XY do not become phantom junctions or nearest-lane jumps. |
| ENG-30 | No-path/runtime failure | Missing edge, closure, blocked exit and stale/unloaded network yield explicit stop/wait/replan states, not straight-line fallback. |
| ENG-31 | Large-world streaming | Lane topology and queries survive actor unloading/pooling; stable IDs and coordinate transforms remain correct. |
| ENG-32 | Save/undo/recovery | Applied edits are atomic transactions; cancel/revert/save/reopen/crash recovery preserve authored data and version history. |
| ENG-33 | Extensible feature type | Register a new typed place/feature with UI, gizmo, validation and export; unknown compiler types cannot publish. |
| ENG-34 | Determinism | Repeated identical builds produce equivalent IDs, mappings and manifests; ordering does not depend on object traversal. |
| ENG-35 | Performance | Agree explicit world-size/agent-count/frame-time budgets, then measure on declared hardware under representative scenarios. |
| ENG-36 | Final usability | Basic UI completes ordinary authoring without Advanced; keyboard/focus/DPI tests and expert workflows both pass. |

## Proposed phase impact, not finalized tasks

| Existing draft | Changes this revision requires |
|---|---|
| Phase 1 | Full hierarchy through point-attached place identity; per-segment-end source binding; simple/contextual shell; stable ownership and transactions. |
| Phase 2 | Directional adapters, before/after profile ownership, mesh-interface checks and shared-topology mesh-owner policy. |
| Phase 3 | Canonical junction/roundabout ports; typed feature summaries; hospital/park/fuel access alongside complete bus/parking/seat payloads. |
| Phase 4 | Lane-specific graph queries, approach-relative exits, service reachability, full 3D independent gizmos and diagnostic selection. |
| Phase 5 | Coherent compiler outputs, controllers and runtime failure handling, streaming, deterministic builds and measured scenario verification. |

These amendments must be reconciled into five final phase files only after the revised prototype is approved. The prior drafts are included unchanged in `prior-phase-drafts/` for traceability.
