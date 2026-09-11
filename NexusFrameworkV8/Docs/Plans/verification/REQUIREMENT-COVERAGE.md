# Requirement-to-test coverage

All twenty current requirements have planned scenario coverage. This mapping checks design completeness; it does not demonstrate engine behavior.

| Requirement | Topic | Phase coverage | Planned tests |
|---|---|---|---|
| R01 | Exact hierarchy and stable identity | 1, 3 | P01-T02, P01-T03, P01-T15, P03-T01 |
| R02 | Landscape authority and source reconciliation | 1, 2, 4, 5 | P01-T04, P01-T05, P01-T06, P01-T07, P01-T08, P01-T09, P01-T10, P01-T13, P02-T10, P04-T08, P05-T12 |
| R03 | Simple, powerful editor | 1, 4, 5 | P01-T01, P01-T10, P01-T15, P04-T11, P04-T12, P04-T15, P05-T15 |
| R04 | Undo/redo, persistence and no silent data loss | 1, 2, 3, 4, 5 | P01-T03, P01-T08, P01-T11, P01-T12, P01-T13, P01-T14, P02-T14, P02-T15, P03-T09, P04-T10, P05-T02, P05-T14 |
| R05 | Four-level per-entry mesh fallback | 2 | P02-T01, P02-T02, P02-T03, P02-T15 |
| R06 | Mesh/profile/port compatibility | 2, 3 | P02-T04, P02-T12, P02-T13, P03-T02, P03-T03 |
| R07 | Explicit lane profiles | 2, 5 | P02-T05, P02-T06, P02-T08, P02-T09, P02-T11, P02-T15, P05-T04 |
| R08 | Directional adapters | 2, 4, 5 | P02-T05, P02-T06, P02-T07, P02-T10, P02-T11, P02-T13, P04-T03, P05-T04 |
| R09 | Typed point features | 2, 3, 4, 5 | P02-T03, P03-T05, P03-T15, P04-T13, P05-T03 |
| R10 | Canonical shared junctions | 2, 3, 4, 5 | P02-T04, P03-T01, P03-T02, P03-T03, P04-T01, P04-T04, P05-T06 |
| R11 | Canonical roundabouts and query semantics | 3, 4, 5 | P03-T04, P04-T05, P04-T06, P05-T06 |
| R12 | Crosswalks and signals | 3, 5 | P03-T06, P03-T07, P05-T07 |
| R13 | Point-attached places | 1, 3, 4, 5 | P01-T07, P03-T08, P03-T10, P03-T12, P03-T13, P03-T14, P03-T15, P04-T07, P05-T03, P05-T09, P05-T10, P05-T11 |
| R14 | Independent 3D gizmos | 3, 4 | P03-T03, P03-T06, P03-T08, P03-T09, P04-T10, P04-T11 |
| R15 | Complete service semantics | 3, 4, 5 | P03-T10, P03-T11, P03-T12, P03-T13, P03-T14, P04-T07, P05-T03, P05-T09, P05-T10, P05-T11 |
| R16 | Legal graph and query contracts | 4, 5 | P04-T01, P04-T02, P04-T03, P04-T04, P04-T06, P04-T07, P04-T08, P04-T09, P04-T13, P05-T05, P05-T08 |
| R17 | Coherent compiler output | 1, 2, 4, 5 | P01-T15, P02-T09, P02-T14, P04-T09, P04-T13, P05-T01, P05-T02, P05-T03, P05-T15 |
| R18 | Actual runtime safety behavior | 5 | P05-T04, P05-T05, P05-T06, P05-T07, P05-T08, P05-T09, P05-T11 |
| R19 | Extensibility and large worlds | 1, 3, 4, 5 | P01-T09, P01-T14, P03-T15, P04-T12, P04-T14, P05-T08, P05-T10, P05-T12, P05-T13 |
| R20 | Evidence and sequential approval | 1, 4, 5 | P01-T01, P01-T14, P04-T15, P05-T13, P05-T14, P05-T15 |

See the current phase files for full procedures and [Test matrix](TEST-MATRIX.md) for statuses. UI and architecture requirements may span several phases; later coverage never excuses a missing prerequisite in an earlier gate.
