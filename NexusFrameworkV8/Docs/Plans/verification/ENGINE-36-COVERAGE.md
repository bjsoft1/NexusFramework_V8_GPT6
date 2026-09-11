# Preserved V8.1 engine acceptance coverage

The 36 engine scenarios in the prior V8.1 contract are retained without treating historical browser tests as engine results. The current five phase plans expand them into 75 scenario tests and add foundational build, recovery, UI and regression cases.

| Prior ID | Prior scenario | Current test coverage | Engine evidence status |
|---|---|---|---|
| ENG-01 | Exact ownership tree | P01-T02, P01-T03 | NOT_EXECUTED |
| ENG-02 | Point versus segment | P01-T04 | NOT_EXECUTED |
| ENG-03 | Landscape reconciliation | P01-T07, P01-T08 | NOT_EXECUTED |
| ENG-04 | 6 → 4 forward | P02-T05, P05-T04 | NOT_EXECUTED |
| ENG-05 | 6 → 4 reverse | P02-T06, P05-T04 | NOT_EXECUTED |
| ENG-06 | 4 → 2 and 6 → 2 | P02-T07, P04-T03, P05-T04 | NOT_EXECUTED |
| ENG-07 | Asymmetric/one-way profiles | P02-T08 | NOT_EXECUTED |
| ENG-08 | Lane-width and count edits | P02-T09, P05-T02 | NOT_EXECUTED |
| ENG-09 | Left/right driving convention | P02-T11, P04-T06 | NOT_EXECUTED |
| ENG-10 | Legal lane changes | P04-T02 | NOT_EXECUTED |
| ENG-11 | Runtime lane-change safety | P05-T05 | NOT_EXECUTED |
| ENG-12 | Junction turns | P03-T02, P04-T04, P05-T06 | NOT_EXECUTED |
| ENG-13 | Shared junction identity | P03-T01 | NOT_EXECUTED |
| ENG-14 | Roundabout identity/count | P03-T04, P04-T05 | NOT_EXECUTED |
| ENG-15 | Roundabout exit query | P04-T06 | NOT_EXECUTED |
| ENG-16 | Roundabout travel | P05-T06 | NOT_EXECUTED |
| ENG-17 | Crosswalk semantics | P03-T05, P03-T06 | NOT_EXECUTED |
| ENG-18 | Signal safety | P03-T07, P05-T07 | NOT_EXECUTED |
| ENG-19 | 3D offsets | P03-T08 | NOT_EXECUTED |
| ENG-20 | Independent sub-point gizmos | P03-T09 | NOT_EXECUTED |
| ENG-21 | Reparent behavior | P03-T09, P04-T10 | NOT_EXECUTED |
| ENG-22 | Parking | P03-T12, P04-T07, P05-T10 | NOT_EXECUTED |
| ENG-23 | Bus stop | P03-T10, P04-T07, P05-T09 | NOT_EXECUTED |
| ENG-24 | Hospital/park/fuel services | P03-T13, P03-T14, P05-T11 | NOT_EXECUTED |
| ENG-25 | Mesh fallback | P02-T01, P02-T02, P02-T03 | NOT_EXECUTED |
| ENG-26 | Mesh interface compatibility | P02-T04, P03-T03 | NOT_EXECUTED |
| ENG-27 | Coherent build output | P02-T14, P05-T02 | NOT_EXECUTED |
| ENG-28 | Geometric containment | P02-T13, P05-T04 | NOT_EXECUTED |
| ENG-29 | Grade separation | P01-T06, P04-T08, P05-T08 | NOT_EXECUTED |
| ENG-30 | No-path/runtime failure | P04-T09, P05-T08 | NOT_EXECUTED |
| ENG-31 | Large-world streaming | P01-T09, P05-T12 | NOT_EXECUTED |
| ENG-32 | Save/undo/recovery | P01-T11, P01-T12, P05-T14 | NOT_EXECUTED |
| ENG-33 | Extensible feature type | P03-T15, P04-T14 | NOT_EXECUTED |
| ENG-34 | Determinism | P04-T14, P05-T01 | NOT_EXECUTED |
| ENG-35 | Performance | P04-T12, P05-T13 | NOT_EXECUTED |
| ENG-36 | Final usability | P01-T15, P04-T15, P05-T15 | NOT_EXECUTED |

The unchanged original is in [V8.1 engine acceptance document](../references/v8-1-review/docs/ENGINE-ACCEPTANCE-TESTS.md). Current scope and gate authority come from the consolidated phase files, not the old draft-status wording.
