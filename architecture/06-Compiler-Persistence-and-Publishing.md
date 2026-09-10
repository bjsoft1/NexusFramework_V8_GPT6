# Compiler, persistence and publication

## One progressively implemented pipeline

```text
Landscape geometry + committed authoring + resolved mesh catalog
                         ↓
          one validated immutable canonical snapshot
                         ↓
     mesh output / lane graph / walk and service graph
                         ↓
       references + spatial/chunk indexes + manifest
                         ↓
        validation → staged output → atomic activation
```

Phase 1 creates the snapshot/version boundary; Phase 2 extends it for road profiles/corridors/previews; Phase 3 adds features/services/intersections; Phase 4 adds query indexes and complete validation; Phase 5 completes coherent runtime publication and consumers. Do not defer every schema/serializer to Phase 5 or create competing temporary models in earlier phases.

## Required output families

World/hierarchy identities; source bindings and fingerprints; points and connected segment ends; segments/sections and sampled geometry; lanes/widths/directions/access; adapter mapping/corridors; junction and roundabout ports/connectors; crossing and control definitions; full bus/parking/seat/hospital/park/fuel payloads; pedestrian/service access; spatial/chunk references; generated-output ownership manifest.

Derived fields may be flattened into the snapshot for runtime efficiency. They are never copied back to authoring descendants as new truth. Visual meshes and lane/service graphs share the same canonical build ID and compatible revisions.

## Version and freshness metadata

Record schema version, compiler version, canonical build ID/content hash, source fingerprints, committed authoring revision, resolved-catalog revision/hash, generator version, coordinate/units convention and fixture/configuration ID. Runtime mutable-state revisions are separate from immutable content hashes.

Use deterministic ordering by stable identity and documented normalization. Exclude wall-clock timestamps and ephemeral machine paths from logical content hashes. A source/catalog mutation during compilation invalidates the staged result rather than silently publishing stale output.

## Safe save and safe build are different operations

Authoring Save persists committed semantics with atomic replacement and recoverable drafts. Build compiles only a consistent captured input revision. Publication activates an internally consistent output set. Successful authoring save does not imply successful build or publish.

Write new generated artifacts to a staging location, verify references/hashes and compatibility, then activate the manifest/pointer through the documented atomic mechanism. The implementation must test failures before and during promotion. Keep last valid output; cleanup must only affect generator-owned stale artifacts after the new result is active and no live consumer needs them.

Never publish a new road mesh with an older lane graph. Never delete hand-authored actors/assets during rebuild. Cancel is a controlled outcome, not an error that damages the previous output.

## Migration and legacy reference policy

Legacy files are evidence of earlier behavior, not executable V8 rules. Inventory supported import versions and retained capabilities in Phase 1. Import into a backup workspace, show the diff and retain original inputs. Unsupported schema/type versions fail or stay preserved/inactive, never silently drop fields.

Preserve stable IDs under compatible edits. Splitting/merging/deleting topology requires explicit remap and dependent-route invalidation. A path referencing an old lane ID must receive a version/remapping decision, not a guessed closest lane.

## Runtime and streaming

Runtime packages contain no editor-module dependency. Logical graphs and state do not depend on visual actors being spawned. Instanced scenery, pooled service proxies and active agents follow distinct budgets. Graph chunks declare availability; actor unload must not be confused with graph deletion.

Use activation/deactivation hysteresis and stable-key state restoration. Record origin/coordinate conversion behavior and test large-coordinate/origin-rebase scenarios. If a query requires unavailable data, return an explicit result and stop/wait/replan rather than assuming a path exists.

## Performance gates

Choose measurable network size, active-agent count, visual actor cap, graph memory, query latency and frame-time budgets in the decision register before claiming scale. Record hardware, build configuration, world fixture, duration and instrumentation. Benchmark a declared dataset, not an empty viewport.

## Evidence and release

A release gate requires build logs, runtime load/launch, scenario results, data-preservation evidence, revision/hash manifests, performance measurements, known limitations and explicit reviewer approval. A browser screenshot or a successful compile alone is not a passed runtime test. Critical data-loss, wrong-way, invalid-route, reference-integrity or mixed-revision failures cannot be waived as visual polish.

R04/R17–R20 are the main requirements. See Phase 5 for exact planned test procedures and expected results.
