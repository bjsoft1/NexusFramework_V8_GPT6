# Decision register and approval gates

**Current state: documentation assembled; prototype approval pending; plan approval pending; no implementation phase started.** The user's request to package documents does not itself authorize coding or approve all earlier prototype behavior.

## Requirements already specified by the user

Exact World/Root hierarchy; point-owned services; location/rotation offsets; Highway→City→State→Root mesh fallback; lane/width/sidewalk/feature information; adapters and intersection types; simple powerful editor; dark/light; real implementation undo/redo; tests and phase-by-phase review. These are carried forward, not asked again.

## Proposed design choices to review with the plan

| ID | Proposed choice | Rationale / decision boundary |
|---|---|---|
| D01 | Use V8.1's six-workspace editor as the default; retain the 60 screens as deeper reference. | Latest user feedback favors less visible complexity. |
| D02 | Point feature switches derive from typed backing components; roundabout is a junction subtype. | Prevent contradictory flag state. |
| D03 | Canonical shared junction/roundabout, explicit mesh-owner highway and scope membership. | Avoid duplicate counts and rebuild-order-dependent meshes. |
| D04 | Highway default plus segment/station profile refs; point before/after display is derived. | Prevent multiple lane-count sources. |
| D05 | Missing catalog entry inherits, broken explicit entry blocks. | Make fallback predictable without hiding invalid assignment. |
| D06 | Meters/seconds/m/s interchange, explicit tested engine/asset conversion; degrees only as an inspector presentation. | Avoid unit and transform ambiguity. |
| D07 | Explicit anchor segment-end/approach and preserve-world/local reparent modes. | Avoid offset errors at branches or banking. |
| D08 | One progressively extended immutable snapshot for render and navigation output. | Eliminate mixed-revision mesh/graph publishing. |

## Implementation setup decisions, not claimed current facts

| ID | Decision to record | Latest gate |
|---|---|---|
| S01 | Exact supported engine version/build, OS/compiler/SDK and target platforms. Prior requested UE version is an intention, not verified compatibility. | Before Phase 1 coding/build. |
| S02 | Concrete native editor integration and module/storage boundaries; adapters chosen after inspecting repository. | Phase 1 schema/API freeze. |
| S03 | Supported legacy migration versions and behavior inventory dispositions. | Phase 1 gate. |
| S04 | Driving side presets, lane-ID/coordinate convention, numeric validation policy and agent dimension classes. | Phase 2 geometry acceptance. |
| S05 | Vehicle-envelope tolerances, curve/taper constraints and approved 6→2 staging. | Phase 2 test setup. |
| S06 | Depth basis when crossing sidewalk/border sides differ; explicit mesh owner for shared topology. | Affected authoring operation; validate before Phase 3 acceptance. |
| S07 | U-turn/reversible-lane capability, control failure policy and traffic-rule presets. Unsupported features block rather than silently degrade. | Before affected Phase 3–5 test. |
| S08 | Query implementation and graph adapter selection, supported runtime hot-update/remap policy. | Phase 4 contract acceptance. |
| S09 | Hardware/world size/agent count/latency/frame-time/memory/actor budgets and test duration. | Before Phase 5 performance execution. |

Do not defer all progress to open decisions. Inspect available repository/engine settings, document a defensible proposed value, and seek approval where the existing gate requires it. Never fabricate a tested engine version or silently override a user requirement.

## Approval order

```text
Prototype review → five-phase plan approval
  → Phase 1 implementation/test/review → explicit approval
  → Phase 2 implementation/test/review → explicit approval
  → Phase 3 implementation/test/review → explicit approval
  → Phase 4 implementation/test/review → explicit approval
  → Phase 5 implementation/test/review → final release approval
```

No automated/batched phase advancement. Rework stays within the current phase until its gate is approved. Approved exceptions must state exact scope; critical data-loss, unsafe-route, reference-integrity and inconsistent-publication failures cannot be labeled harmless polish.

## Proof boundaries

Prior prototype reports are historical browser evidence. This package's checks are documentation/ZIP/browser-guide integrity only. New engine tests remain NOT_EXECUTED. Visual ambition is a product target, not proof of commercial-game parity or absence of every possible defect.
