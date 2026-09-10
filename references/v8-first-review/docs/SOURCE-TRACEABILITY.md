# Source traceability and preservation map

## Inspected input

- Uploaded archive: `NexusFramework_V8(1).zip`.
- SHA-256: `aa0363d9936f94bb75b7abd5b66f140dd1b4c415f0f49a61e12683e429d428ac`.
- The archive contains V0–V7 reference trees; there is no top-level V8 implementation folder.
- Original files were inspected, not modified or executed. The generated prototype is a separate package.
- Historical implementation or audit notes are source evidence of design history, not evidence that V8 passes tests.

| Reference tree | File entries, excluding directory records |
|---|---|
| NexusFramework_V0 | 43 |
| NexusFramework_V1 | 51 |
| NexusFramework_V2 | 51 |
| NexusFramework_V3 | 135 |
| NexusFramework_V4 | 7 |
| NexusFramework_V5 | 118 |
| NexusFramework_V6 | 486 |
| NexusFramework_V7 | 437 |

## References used

| Source path / reference | Requirement carried into the prototype | Screens |
|---|---|---|
| `NexusFramework_V0/Source/NexusFramework/NF/Actors/ANexusRoadAIActor.h` and `ANexusRoadGraph.h`; V7 lessons summary of V0 | Runtime lane travel and connected road graphs remain final outcomes; old actor-owned architecture is not copied. | 44–48, 53 |
| `NexusFramework_V1/Source/NexusFramework/Public/City/CityRoadGraphTypes.h`, `CityDataCompiler.h`, `CityFinalMeshBuilder.h` header inventory; V7 V0–V6 lessons | Graph/compiler/build breadth is preserved as explicit stages, not claimed complete from legacy names. | 24–40, 49–56 |
| `NexusFramework_V7/project-specifications/V0-V6-LESSONS.md` | Preserve proven generation breadth; avoid focus conflicts, incomplete lane links, stale bindings, and false completion claims. | 04–12, 24–60 |
| `NexusFramework_V7/project-specifications/V7-PRODUCT-VISION.md` | Independent Hierarchy/Details/Activity tabs, draft/apply/recover, stable identity, keyboard use. | 01–03, 11–12, 38, 58 |
| `NexusFramework_V7/project-specifications/V7-ARCHITECTURE.md` | Geometry vs semantic authority, runtime/editor separation, versioned schemas. | 04–12, 49–56 |
| `NexusFramework_V7/project-specifications/backlog/B-02-highway-mesh-info-cascade.md` | Mesh data assets; entry-level fallback; null transparency proposal; Highway is deepest scope. | 13–19, 33 |
| `NexusFramework_V7/project-specifications/backlog/B-03-highway-point-attributes.md` | Points are source-derived; preserve authored attributes; resolve feature-summary ownership explicitly. | 06, 08–10, 51 |
| `NexusFramework_V7/project-specifications/backlog/B-05-lane-and-width-data-model.md` | Lane count/width/walkability need an explicit producer/schema home before consumers. | 20–23 |
| Prior V6 master-architecture audit in user Library `logs.txt` | Preserve full Bus Stop/Seat/Parking fields, independent N ≥ 3 gizmos, ConnectedHighwayIds export, parking-specific tests, debug LOD, logical/proxy split. | 30–38, 41–56 |

The prototype does not assert exhaustive semantic parity with every legacy source file. The archive contains 1,328 non-directory entries across eight versions; the selected architecture/backlog documents and relevant header inventory informed this review package. Final phase planning must inventory any additional legacy behavior you require before implementation begins.

## Official interface/API references consulted

Epic documentation was consulted on 2026-09-10 for terminology and future implementation anchors. The layout and icons in this prototype are original work, not redistributed Epic art.

- Unreal Editor Interface: https://dev.epicgames.com/documentation/en-us/unreal-engine/unreal-editor-interface
- Landscape Splines: https://dev.epicgames.com/documentation/en-us/unreal-engine/landscape-splines-in-unreal-engine
- FScopedTransaction: https://dev.epicgames.com/documentation/en-us/unreal-engine/API/Editor/UnrealEd/FScopedTransaction

Engine compatibility, editor builds, actual API integration, and transaction behavior remain untested until the approved implementation phase runs in the chosen Unreal environment.
