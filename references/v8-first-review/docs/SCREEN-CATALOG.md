# V8 — complete 60-screen workflow catalog

Every entry has a physical HTML document in `pages/` and a matching route in `index.html`. Each screen has contextual fields, a three-step guide, and explicit acceptance expectations.

## Workflow 1 / Draft phase 1

| Screen | Title | Review purpose | Page |
|---|---|---|---|
| 01 | World workspace | A single place to author roads, place services, and prepare runtime data. | `pages/01-world-workspace.html` |
| 02 | World root & identity | Create the persistent world boundary; names are never runtime identifiers. | `pages/02-world-root-identity.html` |
| 03 | States, cities & ownership | Organize semantic ownership without duplicating landscape geometry. | `pages/03-states-cities-ownership.html` |
| 04 | Choose landscape sources | Bind one or several landscapes and inspect unloaded cells before scanning. | `pages/04-choose-landscape-sources.html` |
| 05 | Landscape scan inventory | Inspect points, tangents, segment links, and source fingerprints before binding. | `pages/05-landscape-scan-inventory.html` |
| 06 | Scan → diff → reconcile | Pull geometry from Landscape while retaining Nexus codes, lane profiles, and service payloads. | `pages/06-scan-diff-reconcile.html` |
| 07 | Legacy layout import | Preview V0–V7 migration risks without modifying the original source package. | `pages/07-legacy-layout-import.html` |
| 08 | Bind highway control points | Select landscape-owned points and assign highway ownership—not a second spline model. | `pages/08-bind-highway-control-points.html` |
| 09 | Segments & tangent inspection | See curve geometry, lane ownership, and runtime sampling on the same selection. | `pages/09-segments-tangent-inspection.html` |
| 10 | Branches, loops & crossings | Distinguish a connected junction from an overpass that only crosses in projection. | `pages/10-branches-loops-crossings.html` |
| 11 | Draft, compare & apply | Edits remain drafts until validation and impact review succeed. | `pages/11-draft-compare-apply.html` |
| 12 | Save, recovery & conflicts | Show source, base, and recovered draft values without silently choosing a winner. | `pages/12-save-recovery-conflicts.html` |

## Workflow 2 / Draft phase 2

| Screen | Title | Review purpose | Page |
|---|---|---|---|
| 13 | Mesh catalog browser | Use your own mesh assets for roads, junctions, lights, and activation components. | `pages/13-mesh-catalog-browser.html` |
| 14 | Root mesh defaults | Define the shared baseline for every state, city, and highway without copying effective values. | `pages/14-root-mesh-defaults.html` |
| 15 | State mesh overrides | Override only the entries that differ from the root catalog. | `pages/15-state-mesh-overrides.html` |
| 16 | City mesh overrides | Give Riverbend its own bus shelter while retaining shared road and parking defaults. | `pages/16-city-mesh-overrides.html` |
| 17 | Highway mesh overrides | The highway is the deepest mesh-configuration scope; points do not introduce another override level. | `pages/17-highway-mesh-overrides.html` |
| 18 | Live fallback resolver | Resolve each entry at read/generation time: Highway → City → State → Root. | `pages/18-live-fallback-resolver.html` |
| 19 | Missing assets & blocked build | A required missing mesh is a visible blocker—not a silent engine-default replacement. | `pages/19-missing-assets-blocked-build.html` |
| 20 | Lane profile & widths | Define directional lanes, dimensions, rules, and segment-specific transitions explicitly. | `pages/20-lane-profile-widths.html` |
| 21 | Sidewalks, curbs & borders | Separate walkable space from curb geometry and keep road-module boundaries compatible. | `pages/21-sidewalks-curbs-borders.html` |
| 22 | Markings & crossing footprint | Make the crosswalk depth explicit and separate it from its curb-to-curb span. | `pages/22-markings-crossing-footprint.html` |
| 23 | Lane transitions & adapters | Connect 6 → 4 → 2 lanes with explicit merges rather than only stretching a mesh. | `pages/23-lane-transitions-adapters.html` |
| 24 | Road mesh generation preview | Build a derived sample preview from validated source geometry and resolved catalog entries. | `pages/24-road-mesh-generation-preview.html` |

## Workflow 3 / Draft phase 3

| Screen | Title | Review purpose | Page |
|---|---|---|---|
| 25 | T-junction geometry | Three connected arms, smooth corners, correct orientation, and compact modular joins. | `pages/25-t-junction-geometry.html` |
| 26 | Four-way junction & turn links | Inspect lane-to-lane turns independently of the intersection mesh. | `pages/26-four-way-junction-turn-links.html` |
| 27 | Roundabout & circulating lanes | Preserve yield logic, lane continuity, and short square joining arms. | `pages/27-roundabout-circulating-lanes.html` |
| 28 | Traffic light placement | Edit poles, signal heads, offsets, and affected lane groups. | `pages/28-traffic-light-placement.html` |
| 29 | Signal phases & conflict timing | A readable timeline connects vehicle phases, pedestrian clearance, and conflict checks. | `pages/29-signal-phases-conflict-timing.html` |
| 30 | Crosswalk waiting points | Edit start, end, and three or more independent waiting locations. | `pages/30-crosswalk-waiting-points.html` |
| 31 | Bus stop bay & stopping pose | Place a bus stop with an entry, stopping location, exit, and correct lane relationship. | `pages/31-bus-stop-bay-stopping-pose.html` |
| 32 | Passenger waiting & boarding | Author independent waiting, boarding, and alighting locations for a bus service. | `pages/32-passenger-waiting-boarding.html` |
| 33 | Shelter, base & indicator meshes | Bus-stop components resolve from the catalog while placement stays with the activation payload. | `pages/33-shelter-base-indicator-meshes.html` |
| 34 | Seat groups & interaction poses | One mesh may provide several logical seats with explicit capacity and interaction points. | `pages/34-seat-groups-interaction-poses.html` |
| 35 | Parking grid & bay dimensions | Lay out rows and columns with explicit gaps, bay sizes, and inherited mesh components. | `pages/35-parking-grid-bay-dimensions.html` |
| 36 | Parking entry, exit & rules | Connect bays to drivable approach paths and pedestrian exits; occupancy is runtime state. | `pages/36-parking-entry-exit-rules.html` |

## Workflow 4 / Draft phase 4

| Screen | Title | Review purpose | Page |
|---|---|---|---|
| 37 | Universal sub-point gizmo | A common tool supports N points, axis handles, independent transforms, and stable selection. | `pages/37-universal-sub-point-gizmo.html` |
| 38 | Batch transforms & impact preview | Move or rotate selected authored offsets without moving unrelated terrain geometry. | `pages/38-batch-transforms-impact-preview.html` |
| 39 | Lights, signs & roadside fixtures | Place repeated city lights, speed signs, barriers, and bins with reusable placement rules. | `pages/39-lights-signs-roadside-fixtures.html` |
| 40 | Activation registry & custom types | Add advertisement, television, or custom interaction types through a shared contract. | `pages/40-activation-registry-custom-types.html` |
| 41 | Debug layers & always-draw | Keep diagnostics visible outside Landscape Mode without replacing the native tool lifecycle. | `pages/41-debug-layers-always-draw.html` |
| 42 | Debug LOD & performance budgets | Use culling, caps, and selected-item priority to keep large worlds inspectable. | `pages/42-debug-lod-performance-budgets.html` |
| 43 | Validation issue center | Every issue explains its consequence, owner, and next safe action. | `pages/43-validation-issue-center.html` |
| 44 | Highway connectivity graph | Export connected-highway IDs, not only visual meshes or segment endpoints. | `pages/44-highway-connectivity-graph.html` |
| 45 | Directed lane graph | Provide runtime consumers with sampled centerlines, rules, neighbors, and junction connectors. | `pages/45-directed-lane-graph.html` |
| 46 | Lane-change permissions | Lane changes need legal lateral edges, allowed intervals, and runtime collision checks. | `pages/46-lane-change-permissions.html` |
| 47 | Pedestrian & accessible network | Connect sidewalks, waiting areas, crossings, service entrances, and accessible bays. | `pages/47-pedestrian-accessible-network.html` |
| 48 | Routes, destinations & query tools | Preview how AI requests a drivable lane, bus stop, parking bay, or walkable destination. | `pages/48-routes-destinations-query-tools.html` |

## Workflow 5 / Draft phase 5

| Screen | Title | Review purpose | Page |
|---|---|---|---|
| 49 | Compiler preflight & freshness | Reject stale or invalid inputs before building the immutable runtime snapshot. | `pages/49-compiler-preflight-freshness.html` |
| 50 | Runtime compilation pipeline | One canonical compile feeds meshes, traffic, pedestrians, services, and diagnostics. | `pages/50-runtime-compilation-pipeline.html` |
| 51 | Runtime schema inspector | Inspect actual fixture fields for points, segments, lanes, connections, and activation payloads. | `pages/51-runtime-schema-inspector.html` |
| 52 | Mesh build manifest & ownership | Link generated chunks and instances to source IDs and fingerprints for safe incremental rebuilds. | `pages/52-mesh-build-manifest-ownership.html` |
| 53 | Vehicle travel & lane following | A fixture playback illustrates lane following, junction turns, and legal lane-change metadata. | `pages/53-vehicle-travel-lane-following.html` |
| 54 | Pedestrian, queue & boarding flow | Visualize walking → waiting → boarding → alighting with explicit service relationships. | `pages/54-pedestrian-queue-boarding-flow.html` |
| 55 | Parking reservation & occupancy | Runtime reservation is separate from immutable bay layout and survives actor pooling. | `pages/55-parking-reservation-occupancy.html` |
| 56 | Streaming & pooled actors | Keep the full logical world while activating only nearby interactive actor proxies. | `pages/56-streaming-pooled-actors.html` |
| 57 | Verification & regression lab | Collect evidence for five release gates; UI samples are never engine-test evidence. | `pages/57-verification-regression-lab.html` |
| 58 | Appearance, shortcuts & layout | Compact controls with readable labels, dark/light themes, and keyboard navigation. | `pages/58-appearance-shortcuts-layout.html` |
| 59 | Export package & handoff | Review the runtime contract, mesh manifest, source fingerprints, and validation report together. | `pages/59-export-package-handoff.html` |
| 60 | Prototype review & approval | Review screens first, confirm the five-phase plan second, then implement one approved phase at a time. | `pages/60-prototype-review-approval.html` |

