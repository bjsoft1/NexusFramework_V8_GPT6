# Nexus V8.1 — road-network authoring contract

**Proposed design, pending user review.** This is an amendment to the first V8 prototype. It is not a claim that the original project or this browser prototype implements the runtime architecture below.

## 1. Simplify the interface, not the data

The primary surface is one editor: hierarchy on the left, viewport in the middle, selection details on the right. There is no always-open content browser, debug table, task catalog or compiler panel. The six workspace tabs change the tools and inspector context. Advanced reveals technical fields only when needed.

| Workspace | Everyday operation | Advanced/detail coverage retained |
|---|---|---|
| World | Select the world, state, city, highway; choose mesh defaults. | Landscape binding, import/reconciliation, source identity, catalog provenance, draft recovery. |
| Road & lanes | Set lane profiles and adapters. | Segment-end tangents, independent directions, lane identities, widths, restrictions, mesh compatibility. |
| Point features | Add crossing, junction, roundabout or signal features. | Approach ports, per-lane turns, conflict matrices, controllers, waiting points. |
| Places | Add and transform a point-attached service. | Driveways, walk entrances, bays, queues, independent sub-points, service payloads. |
| AI routes | Ask where an agent can travel. | Lane changes, graph debugging, roundabout exits, service reachability, runtime overlays. |
| Verify | Resolve errors and inspect results. | Snapshot/compiler validation, build manifests, runtime tests, streaming, performance and approval evidence. |

The original 60 views remain in `advanced-reference/` as historical detailed reference. They have not all been rewritten. This revision is the proposed everyday entry point. The five prior phase files remain drafts.

## 2. Exact authoring hierarchy

```text
World / Root                         # one record, one top level
  State
    City
      Highway
        Highway Point
          Parking
          Bus Stop
          Hospital
          City Park
          Petrol Pump
          Other registered place type
```

This is the ownership tree, not the driving graph. Segments link highway points; lanes link other lanes; junction records can connect multiple highways. Do not force those many-to-many relationships into duplicated tree nodes.

Use stable IDs independently of display names, array indices, rendering actors and streaming cells. Shared world topology records carry explicit ownership/reference rules for cross-city and cross-state boundaries.

## 3. Points, segments, tangents and profiles

A highway point is a semantic anchor bound to a Landscape control point. A segment connects two anchors. Each connected segment end has its own tangent/connection data. A junction point can have several connected segment ends: it cannot be described safely by one unspecified `Tangent` value.

Suggested record responsibilities:

| Record | Authority and fields |
|---|---|
| Highway | Owning city; point and segment references; default lane profile; mesh-config reference; explicit driving-side convention. |
| HighwayPoint | Parent highway; source control-point binding; connected segment-end references; feature component references; attached places. |
| HighwaySegment | Start/end point IDs; source segment binding; per-end tangent bindings; resolved lane-profile sections; explicit length/station conventions. |
| LaneProfile | Stable lane identities/roles, independent travel directions, widths, median, sidewalk, border, speed and vehicle-class defaults. |
| Adapter | Owner point/interval; input/output profile IDs; transition extent; exact per-direction lane links; taper rules; compatible mesh role/variant. |
| Feature component | Type ID, schema version, owner/anchor, payload, validation rules, compiler handler, gizmo handles and runtime export contract. |

The point inspector displays **lanes before / lanes after** by reading the connected lane-profile sections and adapter. These are not three independently editable copies of lane count.

Store authored semantic settings independently of source geometry. A rescan must not erase lane rules, feature components or service offsets. Source-derived point positions and sampled tangents may be serialized into a compiled runtime snapshot, but not silently turned into a competing authored source of truth.

### Point view example, not a production JSON schema

```text
HighwayPoint P-02
  ParentHighwayId: H-01
  SourceControlPointBinding: Landscape + persistent source ID
  ConnectedSegmentEnds: [S-before.End, S-after.Start]
  DisplayedProfileBefore: 6 lanes (3 forward, 3 reverse)
  DisplayedProfileAfter:  4 lanes (2 forward, 2 reverse)
  ComponentRefs: [Adapter-64, optional crosswalk, optional traffic signal]
  PlaceRefs: [Parking-01, BusStop-01]
  FeatureSummary: derived, read-only
```

If a transition lies inside a source segment, use a defined station interval or a source-aligned split with stable remapping. Do not insert invisible arbitrary discontinuities or corrupt the Landscape source to fit the semantic schema.

## 4. Feature switches must represent real components

`HasCrosswalk` is true when an enabled crosswalk component is linked. `HasAdapter` follows a transition component. `IsJunction` follows membership in a canonical intersection record. In this proposed convention a roundabout is a junction subtype: `IsRoundabout=true` implies `IsJunction=true`.

The simple UI uses a connection type plus optional feature switches; it should not allow contradictory independent `IsRoad`, `IsJunction` and `IsRoundabout` booleans. Crosswalks, traffic lights and suitable adapter intervals may coexist with a junction, subject to geometric and rule validation.

Turning a switch on creates a typed component and opens its minimum required setup. Turning it off performs dependency review, removes the component and repairs/rejects dangling references in one transaction. Imported summaries must be checked against their backing records.

A crosswalk needs geometry, road-edge span, crossing depth, waiting points, pedestrian links and conflict/control references. Preserve the user's sidewalk-plus-highway-border crossing-depth rule; depth and span are distinct dimensions. A traffic light needs controlled lane movements, controller identity, timing and a conflict-safe relationship to crossings. A checkbox alone is not sufficient.

## 5. Lane adapters: the important two-way distinction

For the simple symmetric example:

```text
Physical cross-section: 6 total lanes -> 4 total lanes
Forward travel:          3 lanes -> 2 lanes
Reverse travel:          2 lanes -> 3 lanes
```

The forward direction merges; the reverse direction branches. Do not copy a “merge all directions” rule from the total lane count.

A concrete lane-level example:

| Direction | Input → output | Meaning |
|---|---|---|
| Forward | A.F1 → B.F1 | Continue. |
| Forward | A.F2 → B.F2 | Continue. |
| Forward | A.F3 → B.F2 | Merge with priority/conflict handling. |
| Reverse | B.B1 → A.B1 | Continue. |
| Reverse | B.B2 → A.B2 or A.B3 | Explicit branch; route chooses a legal continuation. |

Each connection requires a continuous lane-center path, usable corridor boundaries, compatible width, participant restrictions, lane-change/merge interval, marking constraints, speed/curvature constraints and priority rules. A visually tapered mesh does not supply these automatically.

General profiles need not be symmetric: 4+2, one-way, bus-only and reversible lanes require explicit policies. Lane numbering must be stable and independent of left/right driving convention. Width changes may occur without count changes. Lane count changes must not be interpolated as fractional lane identities.

Automatic generation may propose legal mappings and compatible mesh variants. It must show those decisions, validate them, and block unresolved cases. It must not silently infer a usable lane from geometric proximity.

## 6. Shared junctions and roundabouts

A junction is one canonical record, not a separate copy in each connected highway point.

```text
Point on Highway A ----\
Point on Highway B ----- Junction J-01
Point on Highway C ----/   ports + lane connectors + rules
```

Each approach identifies its highway, point/segment-end binding, lane profile, entry/exit lane IDs and connection orientation. A source point can be shared geometrically while semantic approach memberships remain explicit.

### Junction payload

Type/preset (T, four-way, skewed, custom), stable approach ports, independent profiles per arm, legal lane-to-lane turn connectors, turn restrictions, signal/stop/yield groups, crosswalk conflicts, usable swept corridors and mesh interface requirements.

### Roundabout payload

Stable roundabout ID, type/preset, circulating lane count, circulation direction, island geometry, entry/exit ports, entry yield rules, ring-lane connectivity, legal exit links per approach/lane/class, markings, pedestrian crossings and signal references where applicable.

`GetRoundabouts(cityId)` returns unique canonical IDs. Four highway references to one roundabout are still one roundabout. `GetRoundaboutExits(roundaboutId, entryLaneId, agentClass, runtimeRevision)` returns reachable allowed exits, not merely the number of meshes around a circle.

Exit numbering is relative to the entry and circulation direction. It must be derived from ordered reachable ports, with an explicit U-turn policy. A four-arm roundabout has four physical ports; after entering one, it does not automatically offer four distinct onward exits. Closures and lane restrictions may further reduce the legal choices.

The tree can show references under points and a secondary shared-topology lookup without changing ownership or duplicating records.

## 7. Point-attached places and offsets

Every place has an owning highway point, a precise anchor frame and a local transform:

```text
PlaceInstance
  Id, TypeId, SchemaVersion
  ParentHighwayPointId
  AnchorFrame = { SegmentId, EndRole, FrameConvention }
  LocationOffsetM = { X, Y, Z }
  RotationOffset = quaternion (Euler degrees in the inspector)
  MeshRole / compatible variant
  VehicleEntranceRefs[], VehicleExitRefs[]
  PedestrianEntranceRefs[]
  InteractionPoints[]                 # each independently identifiable/editable
  TypeSpecificPayload
```

Compose local offsets with the complete anchor transform. Do not add XYZ offsets directly to world coordinates. Transform the offset by the anchor rotation, then add the anchor position. Compose orientation using the documented quaternion/matrix convention. Use full 3D banking/slope information, not yaw-only approximations, in the final implementation.

The browser demo uses a flat, identity-orientation reference frame and a schematic two-pixels-per-meter offset display. It is not a test of sloped 3D transforms. Pitch, roll and elevation are exported review inputs but not visualized in its top-down footprints.

At a junction, the anchor must identify which segment end/approach defines “along” and “right”. Optional station-based anchoring along a curved segment must be an explicit frame mode; a large straight tangent X offset is not equivalent to following the curve for the same distance.

Reparenting in the production editor offers an explicit preserve-world or preserve-local choice, with transaction/dependency preview. This browser prototype preserves local offsets and labels that behavior. Source rotation or movement recalculates derived world placement without losing authored local offsets.

| Place type | Required authoring beyond a mesh |
|---|---|
| Parking | Entry/exit lanes, rows/columns, gaps, bay dimensions and IDs, bay poses, allowed vehicle classes, approach/exit corridors. |
| Bus stop | Entry, stopping and exit poses; bus-lane/bay access; waiting, boarding/alighting and seat points; shelter/base/indicator meshes. |
| Hospital | Vehicle entrances, ambulance drop-off/access restrictions, parking/service connections and pedestrian doors. |
| City park | Walk entrances, internal pedestrian links/areas, seats and optional controlled vehicle/service access. |
| Petrol pump | Driveway entry/exit, circulation, fuel-bay poses, queues, compatible vehicle/service categories. |

A service's building-center position is never a substitute for its entrance network. Dynamic occupancy, reservations and signal phases live in separate runtime state rather than in the authored placement/config asset.

## 8. Mesh fallback and the navigation contract

Resolve each required mesh entry in this order:

```text
Highway -> City -> State -> Root
```

Point and service components choose a semantic role/type/profile variant from that resolved catalog. They do not introduce another catalog inheritance level. A missing child entry falls through; a broken explicitly assigned asset is diagnosed instead of silently hidden.

A multi-highway junction/roundabout requires an explicit mesh-owner policy, such as `MeshOwnerHighwayId`. Do not let whichever approach happens to rebuild last decide its material/mesh. The selected owner's cascade resolves the entry; every approach port must still pass profile/width/socket/interface compatibility checks. This owner choice is a proposed Advanced setting for approval.

An adapter catalog entry should describe compatible source/target profiles, boundaries, orientation, pivot, units and port requirements. A four-lane approach cannot accept a two-lane mesh just because the asset has a similar name. The same principle applies to roundabouts and mixed-profile junctions.

## 9. Compile once into consistent outputs

```text
Landscape source geometry + authored semantics + resolved mesh catalog
    -> validated, versioned canonical snapshot
       -> render/generation output and ownership manifest
       -> lane graph, junction/roundabout connectors and drivable corridors
       -> pedestrian graph, crosswalks and service access/interaction data
       -> spatial indexes, chunk/streaming references and query metadata
```

Stamp outputs with source revision/fingerprint, catalog revision, schema version and compiler version. Validate before atomically publishing all related outputs. Keep the last valid output when the new build fails. Never ship a new road mesh with the previous lane graph.

Stable IDs survive compatible edits; splitting/merging/removing source elements requires explicit remapping or a blocking reconciliation issue. Unloaded source cells are not proof of deletion. Runtime graph availability must not depend on a decorative actor currently being rendered.

## 10. Runtime queries and responsibilities

Proposed query surface:

```text
FindRoute(startLane, destinationAccess, agentClass)
GetLaneSuccessors(laneId)
GetLegalLaneChanges(laneId, distanceAlongLane, agentClass)
GetUpcomingFeatures(route, lookAheadDistance)
GetJunctionMovements(junctionId, entryLaneId, agentClass)
GetRoundabouts(worldOrCityId)
GetRoundaboutExits(roundaboutId, entryLaneId, agentClass)
FindReachablePlace(type, fromLane, agentClass)
GetServiceEntrance(placeId, participantType)
GetLaneLocation(laneId, distanceAlongLane)
```

Legal lane-change permission is not permission to execute immediately. The controller must also check space, predicted traffic conflicts, headway, speed, braking distance, vehicle dimensions and the current runtime state. Use lane IDs and distance-along-lane tracking, with elevation/direction-aware projection and bounded recovery, not an unconstrained nearest-road search.

The lane network plans legal motion; a trajectory/controller layer carries it out while respecting lane corridors and current obstacles. Pedestrian navigation may use a separate walk graph/navmesh with explicit crossing links. Do not substitute an unrestricted pedestrian navmesh for car lane-following.

No-path, closed-lane, stale-network and unloaded-next-cell results are explicit states. Stop safely, wait for data or replan as appropriate. Do not invent a straight-line destination path, teleport to a nearby road, or repeatedly clamp a vehicle into a lane to hide a controller error.

The browser implements only a small forward-lane BFS fixture, including a missing-merge and closed-exit example. It does not implement these full runtime APIs, reverse routing, lane-change planning, vehicle avoidance or complete service reachability.

## 11. Dynamic/extensible without becoming opaque

Use a versioned type registry for point features and places: display information, default payload, editor fields, validation, gizmo handles, mesh-role resolver and compiler/runtime contract. New types should reuse this infrastructure rather than require a separate giant editor page.

Typed extension points must reject unknown/unhandled runtime payloads at publish time. “Dynamic” must not mean accepting arbitrary unvalidated JSON, executing untrusted scripts or silently dropping unsupported information.

Use human-readable defaults and presets in Basic, with decisions visible and overrideable in Advanced. Record provenance for inherited values and generated choices. Keep error messages actionable: identify the exact point/lane/port and provide a Select/Fix action.

## 12. Evidence and limits

The desired visual/simulation ambition is a product goal, not proof of reliability or parity with any commercial game. The enforceable target is explicit contracts, measurable geometry/navigation checks, repeatable integration tests and runtime diagnostics.

No configuration can guarantee the absence of every runtime defect. The acceptance target is to reject known invalid networks before publishing, preserve coherent outputs, handle no-path/runtime failure states safely and demonstrate the agreed scenarios in the actual engine.

See `ENGINE-ACCEPTANCE-TESTS.md` for required future tests. All those engine tests remain **UNEXECUTED**.

## Primary-source context (consulted 2026-09-10)

These references support terminology and the separation of geometry, connectivity and travel rules. They do not imply a dependency choice or production compliance for Nexus.

- Epic Games, Landscape Splines: https://dev.epicgames.com/documentation/en-us/unreal-engine/landscape-splines-in-unreal-engine — points versus segments; per-segment-end tangent editing.
- ASAM OpenDRIVE overview: https://www.asam.net/standards/detail/opendrive/ — road geometry, lane links, junction connections and static road-network descriptions.
- FZI Lanelet2 routing documentation: https://github.com/fzi-forschungszentrum-informatik/Lanelet2/blob/master/lanelet2_routing/README.md — successor/adjacent/lane-change/conflict relations, participant-specific routing and explicit no-path results.
- Epic Games, FZoneGraphLaneLocation: https://dev.epicgames.com/documentation/en-us/unreal-engine/API/Plugins/ZoneGraph/FZoneGraphLaneLocation — lane handle, distance-along-lane, position, direction and tangent query representation.
