# Lane graph and runtime query contract

**Design interface, not a delivered C++ API.** Concrete structs, query engine and coordinate adapter are chosen and tested in implementation without weakening these semantics.

## Three related networks

The highway graph answers which highways connect. The directed lane graph answers where a particular vehicle class may travel and change lanes. The pedestrian/service graph answers where an agent may walk, wait, board or access a service. A connection in one is not automatic permission in the others.

A lane includes stable identity, owning section/profile, travel direction, length/station convention, continuous center geometry, usable boundary corridor, speed/access restrictions, successors/predecessors and lateral neighbors. Junction/adapter connectors are explicit traversable edges with their own geometry and constraints. An adjacent lane can be unreachable because of a solid marking or rule.

A point/mesh flag never substitutes for edge geometry or access rules. Auto-generated suggestions must be validated against profiles, terrain/port boundaries and supported agent envelopes.

## Proposed queries

| Query | Contract |
|---|---|
| `FindRoute(startLane, station, destinationAccess, participant, revision)` | Legal ordered lane/connector path ending at a defined entrance/access target; returns explicit failure. |
| `GetLaneSuccessors(laneId, participant, revision)` | Approved direction/class-compatible onward edges. |
| `GetLegalLaneChanges(laneId, station, participant, revision)` | Allowed target lanes and intervals; not a real-time safe-gap decision. |
| `GetLaneLocation(laneId, station, revision)` | Position/frame/direction within declared lane bounds, with out-of-range policy. |
| `ProjectToLane(pose, participant, candidateScope, revision)` | Bounded elevation/heading-aware match; fails rather than snapping to unrelated roads. |
| `GetUpcomingFeatures(route, lookAheadDistance, revision)` | Ordered crossing, adapter, signal, intersection and service features relevant to the route. |
| `GetJunctionMovements(junctionId, entryLane, participant, revision)` | Legal incoming-to-outgoing movements and control/priority references. |
| `GetRoundabouts(scopeId, revision)` | De-duplicated canonical objects in explicit world/city scope membership. |
| `GetRoundaboutExits(id, entryLane, participant, revision)` | Reachable permitted exits ordered relative to entry/circulation, with U-turn policy. |
| `FindReachablePlace(type, fromLane, participant, revision)` | Places with a valid access route, not nearest meshes. |
| `GetServiceEntrance(placeId, participant, revision)` | Eligible drive/walk entrances and interaction approach targets. |

`revision` identifies immutable network data plus the applicable runtime rule/state revision. A consumer cannot combine paths from one topology version with geometry from another. In-flight agents require explicit compatibility/remapping or controlled replanning when a published graph changes.

## Failure states

| Result | Meaning | Consumer expectation |
|---|---|---|
| OK | A result exists for the requested snapshot and rules. | Track its revision and execute only after live safety checks. |
| INVALID_REQUEST | Missing/invalid identity, station, schema or participant. | Report/reject; do not guess an alternative target. |
| NO_PATH | Available valid data contains no legal route. | Stop safely or choose a different legal destination through policy. |
| CLOSED | Required access/lane is closed under current rules. | Wait or replan, preserving lane constraints. |
| DATA_UNAVAILABLE | Required graph chunk/source is not loaded/available. | Request data and wait/replan before entering unknown geometry. |
| STALE | Query/input output revisions are not compatible. | Refresh/replan; never silently use mixed revisions. |

No unrestricted straight-line fallback to a destination. No nearest-lane teleport across an overpass. No repeated hard clamping to disguise bad controller geometry. Debug recovery tools may exist only as explicit non-production diagnostics with recorded events.

## Controller responsibility

The graph grants legal choices. A trajectory/controller verifies current traffic gaps, predicted collisions, speed, braking, vehicle dimensions, turn radius, signal phase and corridor containment. A statically legal merge may need to wait. Runtime changes can invalidate a formerly valid route; that is an expected condition, not an excuse to leave the road.

Validate the swept vehicle envelope, not only a center sample. Declare sampling/sweep method and tolerances; record speed and agent dimensions. Test opposite travel directions, asymmetric profiles, curves, banking, stopped traffic, blocked exits and missed roundabout exits. A demonstration with one small car does not prove buses fit the same connector.

Pedestrian motion may use an approved walk graph/navmesh adapter, with explicit crossing/waiting links and access restrictions. An unrestricted pedestrian path representation is not sufficient for vehicle lane-following.

## Runtime state and graph availability

Immutable topology is independent of decorative actor lifetime. Graph chunks have their own load state; it is valid to return DATA_UNAVAILABLE rather than pretending every graph is always resident. Mutable closures, signal execution, reservations and logical agent states are keyed by stable identity.

A parking lease needs owner, expiry and version; actual occupancy is separate from lease expiry. Contention must be exclusive and idempotent. Pooled actor deactivation must not create duplicate capacity or release occupied resources.

## Acceptance anchors

R08/R10/R11/R15–R19. Phase 4 validates query results and negative fixtures; Phase 5 validates moving agents, safe failure handling and mutable state. None of these engine checks have run in this documentation task.
