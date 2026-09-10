# Mesh inheritance, lane profiles and adapters

**Proposed V8 implementation contract.** Existing meshes are supplied/configured through the system; visual presence alone does not establish valid navigation.

## Four scopes, resolved independently per entry

```text
Highway → City → State → Root
```

The lookup key identifies a semantic role, compatible profile/variant and optional component slot. For example, a City bus-shelter override must not hide a Root road mesh. Points and places pick the role/variant they require from the resolved catalog; they do not add another catalog-assignment level.

| Assignment state | Resolution behavior |
|---|---|
| Config reference absent | Continue to parent. |
| Config exists but requested entry is absent | Continue to parent for that entry only. |
| Explicit entry exists and asset is valid/compatible | Use it; show source scope and revision. |
| Explicit entry refers to a missing/broken/incompatible asset | Report a blocker at that scope; do not hide it with a parent asset. |
| Optional feature is explicitly disabled | Do not request its mesh; feature enablement is separate from missing catalog assignment. |
| No scope supplies a required entry | Block preview/publication requiring that entry; keep prior valid output. |

Effective values are derived. Clearing an override reveals the parent; it does not paste parent values into the child. Parent changes invalidate only derived dependents not shadowed by a nearer override.

## Shared intersection mesh owner

A junction/roundabout defines one explicit `MeshOwnerHighwayId` referencing a valid participating approach highway. That highway's cascade resolves its shared mesh/material roles. The editor may propose an owner when creating the intersection, but must expose and persist the choice. Rebuild order never selects ownership.

If the owner is removed, reassignment is reviewed or publication is blocked. Every approach must still pass independent compatibility checks: assigning a four-lane mesh from one highway cannot legitimize an incompatible two-lane arm.

## Mesh interface metadata

Each supported mesh variant declares role/type, units, orientation, pivot, boundary/port frames, lane-profile compatibility, road/sidewalk/border widths, material slots and supported deformation/curvature assumptions. Validate geometry rather than treating similar asset names as compatibility.

Preserve the reviewed modular requirements: compatible open join ends, no unwanted bottom/closing faces, consistent visible-side normals, smooth junction corners with short useful arms, shared sidewalk/border conventions and texture-based lane markings. Junction conflict-area markings are type-specific; do not automatically extend a highway center line through every intersection.

The system consumes approved meshes/configurations. Photorealistic asset creation is not implied by this documentation package.

## Lane-profile producer

Highway has a default profile; each explicit segment/station section can reference another profile. The point inspector reads connected section data. A lane record has stable ID/role, travel direction, width or explicit width function, speed rule, allowed participant classes, markings and usable boundaries.

Profiles also define median, sidewalk/walkability, curb and border dimensions. Width zero may disable an optional sidewalk according to policy; lane usable widths cannot be nonpositive. Reject NaN, infinite or unsupported values. Count is derived from actual lane records, never from fractional interpolation.

Driving side is explicit. Lane ID must not equal a mutable left-to-right array index. Asymmetric, one-way and bus-only roads are first-class test fixtures. Reversible/time-dependent lanes require a declared supported capability and direction-state transition policy; until supported, reject such a profile rather than pretending it is a static lane.

## Directional 6 → 4 → 2 example

For a symmetric road with three lanes each way narrowing to two each way:

| Travel | Mapping example | Required meaning |
|---|---|---|
| Forward | A.F1 → B.F1 | Continue. |
| Forward | A.F2 → B.F2 | Continue. |
| Forward | A.F3 → B.F2 | Explicit merge with corridor, permitted interval and priority. |
| Reverse | B.R1 → A.R1 | Continue. |
| Reverse | B.R2 → A.R2 or A.R3 | Explicit branch; route chooses a legal continuation. |

The next 4→2 stage is a two-to-one merge in one direction and one-to-two branch in the other. A 6→2 preset must define direct mappings with sufficient geometry or approved staged merges. A generic label is not enough.

Adapter payload includes anchor point, relevant segment/station range, start/end profiles, per-direction links, continuous center paths, corridor boundaries, marking restrictions, merge priority, curvature/speed constraints and mesh-role interface. Dropped lanes require a legal successor or an explicit approved terminal condition; a mesh taper is not a successor.

Generate proposed mappings from presets only when they can be inspected and validated. A false-positive connection based on distance or shared XY is prohibited. A changed width/count invalidates render and graph data together.

## Acceptance anchors

R05–R08 and R17. Primary tests: P02-T01–P02-T15; route use P04-T03; real motion P05-T04/P05-T05. All are planned engine tests, not already executed results.
