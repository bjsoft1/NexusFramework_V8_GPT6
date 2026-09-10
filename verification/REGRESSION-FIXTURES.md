# Regression fixture catalogue

**Planned fixtures; no Unreal assets or executed fixture results are delivered in this documentation package.** Each implementation fixture must record IDs, units, scale, seed, source revision and expected connectivity. Browser sample data remains illustrative only.

| Fixture | Required setup | Important negative variants |
|---|---|---|
| FX-01 Ownership/source | One world, two states, several cities, point-owned service shells, shared source point, loop and rotated Landscape actor. | Cycle, orphan, duplicate import, stale map binding, unloaded partition. |
| FX-02 Source edits/recovery | Authored codes/features/offsets on a road; move/split/merge; saved revision plus draft. | Lost mapping, interrupted save, stale draft, unsupported schema, false deletion. |
| FX-03 Profile cascade | Root road/border/shelter; partial State/City/Highway overrides and explicit shared mesh owner. | Missing all entries, dangling override, wrong port/width/pivot, deleted mesh owner. |
| FX-04 Adapters | Symmetric 6→4→2 road, both directions; supported 6→2; width-only, one-way 2+0, asymmetric 4+2, bus lane. | Missing successor, wrong-way link, insufficient taper, incompatible participant, invalid station range. |
| FX-05 Mesh corridor | Straight, smooth curve, banked/graded segment, adapter and shared boundary using existing mesh catalog. | Misaligned sidewalk/border, unwanted join cap, invalid normals, vehicle envelope outside road. |
| FX-06 Shared junction | T, four-way and skewed/mixed-profile arms spanning cities, rotated intersection, legal turns and U-turn policy. | Duplicate canonical object, prohibited turn, mesh-owner order dependence, wrong port orientation. |
| FX-07 Roundabout | Single-/multi-lane rings, four arms, both approved circulation conventions, multiple highway references. | Duplicate count, invalid exit order, unreachable exit lane, closure, missed exit, forbidden U-turn. |
| FX-08 Crossing/control | Crossing full span and separate depth, unequal-side dimensions, three waiting handles, movement groups and phase definitions. | Ambiguous depth basis, missing walk link, incompatible greens, missing/stale controller, bad clearance. |
| FX-09 Bus/seats | Bus bay entry/stop/exit, queue/board/alight, shelter/base/indicator, two benches with three seats each. | Sibling handle drift, missing bus/walk access, duplicate reservation, unsupported capacity. |
| FX-10 Parking | 2×6 grid, nonzero gaps, angled/accessible bays, different vehicle classes, walk access and independent bay IDs. | Missing exit, oversized vehicle, competing reservations, expired lease on occupied bay, proxy unload. |
| FX-11 Other places | Hospital with ambulance-restricted access, city park walking entrances, petrol pump queues and fuel bays. | Route to mesh center, forbidden vehicle class, blocked entrance, missing service compatibility. |
| FX-12 Frames/gizmos | Banked/sloped anchor, nonzero XYZ and pitch/roll/yaw, N≥3 independent handles, both reparent modes. | Wrong approach at a junction, yaw-only math, array-index identity, mode change during drag. |
| FX-13 Query failure | Directed graphs with parallel roads, overpass, loops, dead end, closures and unavailable chunks. | Nearest-road jump, mixed revision, unknown ID, forged straight-line fallback, wrong lane-change permission. |
| FX-14 Publish/runtime | Complete authored route through adapters, intersections, roundabout and each service; coherent manifests. | Source changes mid-build, interrupted promotion, corrupt reference, unsupported runtime type, stale graph. |
| FX-15 Scale/streaming | Declared large world with seeded topology, graph chunks, pooled visuals, stateful services and moving agents. | Actor cap violations, graph disappearance with visuals, state loss, origin rebase error, tail-latency spikes. |

## Measurement protocol

Before geometry/traffic execution, declare agent classes and dimensions, speed envelope, legal corridor policy, coordinate conversion and tolerance values. Include buses or other wide/long agents where the road permits them. Store swept-volume/sampling resolution and repeatability conditions; sparse center-point checks are not enough to establish footprint containment.

Before performance tests, record target hardware, engine/build configuration, dataset size, active agents, proxy count, duration, warm-up and metric definitions. Set pass thresholds before observing results. Do not invent a passed budget from a small empty example.

## Fault injection and rollback

Inject invalid input and interruption into test copies only. Preserve originals and record hashes. Deliberately bad variants must fail at the expected stage with exact entity/port/lane diagnostics. A negative fixture is not successful because it accidentally produced a plausible-looking road.
