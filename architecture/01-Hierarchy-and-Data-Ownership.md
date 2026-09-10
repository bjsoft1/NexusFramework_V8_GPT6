# Hierarchy and data ownership

**V8.1-aligned proposed implementation contract.** Approval is pending. This document defines responsibilities, not an implemented engine API.

## Exact ownership tree

```text
World / Root                         one entity, two display names
└── State
    └── City
        └── Highway
            └── Highway Point
                ├── Parking
                ├── Bus Stop
                ├── Hospital
                ├── City Park
                ├── Petrol Pump
                └── Other registered Place
```

The hierarchy expresses ownership and selection. It does not replace the road graph. Segments connect points; lane edges connect lanes; shared intersections connect highway approaches. Never duplicate a junction just to make it fit below two different highways.

## Responsibility boundaries

| Record or layer | Owns | Must not own |
|---|---|---|
| Landscape source | Source control points, connected segment ends, tangents and terrain geometry. | Nexus place offsets, service behavior, mesh override cascade or mutable traffic state. |
| World/State/City | Ownership, stable IDs, display information and optional mesh-config assignment. | Duplicated inherited effective catalog values. |
| Highway | Parent city, source bindings, point/segment refs, default lane profile, driving-side policy, mesh config. | Private copies of shared intersection topology. |
| HighwayPoint | Parent highway, source-control-point binding, connected segment-end refs, features and child places. | One ambiguous tangent for every connected segment; a fifth mesh-inheritance level. |
| HighwaySegment/section | End-point bindings, source-segment binding, per-end geometry, station convention, lane-profile sections. | Independent source coordinates editable alongside Landscape. |
| Shared topology | Canonical junction/roundabout ID, approach memberships, explicit mesh owner, lane connections and rules. | A second instance per referencing highway. |
| Feature/Place | Typed/versioned semantics, anchor frame, local offsets, component/interaction references. | Live occupancy or reservations baked into the asset. |
| Compiled snapshot | Immutable derived geometry, resolved catalogs, graphs, runtime payloads and revision metadata. | Uncommitted source edits or an alternative authored geometry model. |
| Runtime state | Closures, signal execution, reservations, occupancy and logical agents. | Structural writes to the authoring source as agents move. |

## Points are not segments

A point is a semantic anchor bound to a source control point. A segment joins anchors; each connected end has its own tangent and frame. A three-arm junction needs at least three end bindings, not one `Tangent` value.

The inspector's **Before / After** labels are views of connected profile sections. They are not independent count fields. Where more than two segment ends exist, choose the approach being inspected instead of silently picking an arbitrary before/after pair.

A transition inside one source segment uses an explicit station interval. If an actual source split is needed, perform it through the approved source-editing workflow with stable remapping; never introduce an invisible geometry mutation during save or scan.

## Identity, references and boundaries

All logical entities and independently editable handles use stable GUIDs. Names and developer codes are readable helpers; array order, actor pointers and mesh instance indices are not identities. Proposed code uniqueness is within the owning namespace; the implementation records the chosen rule before schema freeze.

A source point may be referenced by more than one semantic approach. Its geometry binding is canonical; highway ownership and approach identity remain explicit. A junction spanning cities/states has one primary owner for editing and an explicit scope-membership set for discovery. `GetRoundabouts(cityId)` de-duplicates canonical IDs within that scope. Summing city counts is not a valid world-unique count.

Deleting/reparenting an owner requires an impact preview for features, places, graphs and shared-topology references. The user must see dependent objects before any destructive commit. No silent orphan cleanup.

## Reconciliation and data safety

Scan resolves source facts; Diff compares with the prior binding revision; Review chooses accepted changes/remappings; Apply commits only accepted semantic changes. Save writes semantics, not terrain. Native source tools remain authoritative for geometry editing.

Source movement recomputes derived positions without erasing offsets, rules or feature payloads. Source deletion/split/merge produces an explicit remap or blocker. Unloaded cells are unavailable, not deleted. Unknown imported payloads remain preserved and cannot be published without a compatible handler.

## Proposed numeric conventions

Canonical interchange lengths use meters, time uses seconds and speed uses meters per second. Inspector speed can display km/h and rotation can display degrees. Quaternions/matrices use a documented tested convention. The engine adapter records coordinate handedness, axis meaning and exact unit conversions; never infer them from an asset's filename.

The source transform, local actor transform, road-reference direction and travel direction are different concepts. Normalize supported transformations before deriving frame/corridor data; reject unsupported mirrored/nonuniform-scale cases rather than guessing.

## Acceptance anchors

R01/R02/R04. Primary tests: P01-T02 through P01-T14; banked place frames P03-T08; cross-boundary shared topology P03-T01 and P04-T05.
