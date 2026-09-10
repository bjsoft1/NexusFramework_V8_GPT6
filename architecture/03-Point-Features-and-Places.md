# Point features, intersections and attached places

## Simple summaries backed by real records

| Inspector summary | Source of truth |
|---|---|
| Has Crosswalk | An enabled linked crosswalk component with required payload. |
| Has Adapter | A linked directional transition component/interval. |
| Is Junction | Membership in a canonical intersection record. |
| Is Roundabout | Intersection type is a roundabout subtype; also implies Is Junction. |
| Has Traffic Light | Linked signal/control components, not the mesh alone. |

A switch creates/removes a typed component in one transaction. Enabling opens minimum required setup. Disabling shows dependent paths/controllers/waiting points before changing references. Roundabout, crosswalk and signal components may coexist where geometry and rules allow. Contradictory standalone booleans are not a valid data model.

The registry defines type ID/version, display data, defaults, Basic/Advanced fields, validators, independently identifiable gizmo handles, mesh roles, serializer and compiler/runtime adapter. Preserve unknown input data, but reject publication when a required type has no known handler. Do not run arbitrary uploaded code from payload fields.

## Shared junctions and roundabouts

One canonical junction contains typed approaches, each with highway/point/segment-end references, an oriented port, incoming/outgoing lane IDs and a profile. Lane-to-lane connectors carry turn type, geometry, usable corridor, participant restrictions, priority/control and conflict relationships.

Roundabouts add ring-lane topology, circulation direction, island/entry geometry, per-entry yield policy, legal exit connectors and U-turn policy. Count the canonical ID, not each highway's membership. Exit ordering is entry-relative along circulation and filters unreachable/prohibited/closed exits using the requested rule revision. Physical arm count, topology exit ports and available route exits are three distinct quantities.

Mixed-width arms are valid only when mesh interfaces and lane connectors explicitly support them. Grade-separated crossings are not junctions merely because their plan views overlap. One canonical intersection transform is used by mesh ports, signals, connectors and offsets.

## Crosswalk dimensions and signal relationships

**Span** crosses the full usable road between the selected curb-side road edges. **Depth** lies along the crossing footprint's road-travel axis and follows the reviewed sidewalk-width-plus-highway-border-width rule. Store both explicitly and display their names and axes.

For unequal sidewalk/border widths, the setup records which approach/side supplies the depth rule or asks for an explicitly approved override. It must not silently average the sides. The coverage preview shows the entire footprint and identifies invalid curb/walk connections.

Crosswalk payload includes endpoints/footprint, independent waiting poses, walk entrances/exits, accessibility flags, controlled/conflicting vehicle movements and controller references. Signal payload includes pole/head mesh roles and offsets, controller, movement groups, phase definitions, clearance intervals and conflict validation. Live phase state belongs to runtime, not the authored definition.

## Place base fields

```text
Id, TypeId, SchemaVersion
ParentHighwayPointId
AnchorFrame { SegmentId, EndRole or explicit Station, FrameConvention }
LocalLocationMeters { X, Y, Z }
LocalRotationQuaternion (Euler degrees displayed)
MeshRole / compatible variant
VehicleEntranceRefs[], VehicleExitRefs[], PedestrianEntranceRefs[]
InteractionPoints[] { stable Id, role, local pose, restrictions }
TypeSpecificPayload
```

Every place remains a point-owned child even when a station-based frame mode references a nearby segment interval. At multi-arm points, the selected approach frame is mandatory. Offsets do not replace an explicit path from the road to the entrance.

| Place family | Required additional information |
|---|---|
| Bus stop | Bus access lane/bay; entry, stopping and exit poses; queue/waiting points; boarding/alighting; shelter/base/indicator roles; seat interactions and passenger walk links. |
| Seat group | Physical instance count; logical seat count/capacity; row/column or grouping layout; spacing; independent sitting/approach poses; optional supported mesh-by-capacity roles. |
| Parking | Rows/columns; row/column gaps; bay width/length/angle; stable bay IDs and poses; allowed classes; accessible bays; base/indicator roles; entrance/exit and pedestrian corridors. |
| Hospital | Vehicle/ambulance entrance restrictions, drop-off/service access, parking connections and separate pedestrian doors. |
| City park | Walk entrances and internal links/areas, seats and optional explicitly controlled maintenance/vehicle access. |
| Petrol pump | Driveway entry/exit, circulation, fuel-bay poses, queue/wait positions and fuel/vehicle-category compatibility. |

All service component meshes resolve through the parent highway's catalog cascade. A service payload can choose a role/variant but cannot silently create a private override level.

A bus-stop test does not prove parking is complete. Parking, bus and seat payloads require independent field inventories, round-trips and behavior fixtures. Multiple logical seats can share a physical bench mesh.

## Full 3D local frames

With an explicitly documented column-vector transform convention:

```text
T_world_place = T_world_anchor × T_anchor_place
T_newAnchor_place = inverse(T_world_newAnchor) × T_world_place   # preserve world
```

Preserve-local reparent keeps `T_anchor_place` unchanged and recomputes world placement. Preserve-world recomputes local placement as above. Use the complete orientation (banking/slope included), not just yaw. Pure local X offset on a curved anchor tangent is not distance travelled along the curve; station mode must be explicit.

The anchor's reference direction defines axes independently from a vehicle travelling backward along the segment. Store and test conversion conventions at the engine boundary. Source geometry may change without overwriting authored local offsets.

Move/rotate each sub-point independently using a stable handle ID. Scale is offered only where a schema explicitly supports dimensions/scale; do not allow arbitrary visual scaling that leaves collision or lane corridors unchanged.

## Mutable state boundary

Reservations, occupied bays, fuel service progress, seat allocation and active signal phase are keyed by logical IDs in runtime state. Visual actors are proxies. Deactivating a proxy does not delete capacity, free an occupied bay or erase an agent reservation.

## Acceptance anchors

R09–R15, R19. Phase 3 tests cover authoring/serialization; Phase 4 covers queries; Phase 5 covers real service behavior, traffic control and runtime state.
