# Editor UX and gizmo contract

## Main editor, not sixty mandatory steps

Use the V8.1 simplified prototype as the everyday entry point. Keep one hierarchy on the left, a dominant central landscape viewport and a selection-specific inspector on the right. A small toolbar and unobtrusive status line are persistent. Content, logs, compilation and diagnostics open when requested, not as four permanent competing panels.

| Workspace | Everyday task | Advanced content |
|---|---|---|
| World | Choose owner/source and mesh defaults. | Source diff, migration, schema/provenance, recovery. |
| Road & lanes | Pick lane profile and approved adapter. | Per-end tangents, widths, restrictions, explicit mapping, mesh interfaces. |
| Point features | Add crossing or choose intersection type. | Per-lane connectors, controllers, conflicts and port frames. |
| Places | Add parking/bus/hospital/park/fuel and move/rotate it. | Entrance networks, sub-point arrays, bay/seat/queue layouts. |
| AI routes | Test legal travel and service access. | Graph overlays, lane-change intervals, unique roundabouts and exit queries. |
| Verify | See actionable problems and build evidence. | Compiler revision/manifest, failure injection, runtime and performance evidence. |

The original 60-screen design is a reference catalog of deeper capability, not the mandatory primary navigation. Preserve useful features while reducing default visible fields. Display minimum required fields, sensible presets, clear effective values and an Advanced disclosure. User-defined types reuse the registry and inspector infrastructure instead of each requiring a large new permanent page.

## Everyday guided workflow

Select source and hierarchy; resolve a highway mesh/profile; inspect point features and adapters; add places using a chosen frame; query a route; fix validation blockers; preview/build a coherent snapshot. Show the next action only when prerequisites are clear. Empty and error states name the missing setup instead of showing a blank list.

## Visual and accessibility rules

Target 1920×1080 landscape first. Compact controls need readable labels/tooltips, visible focus and sufficient hit targets. Match the UE-inspired visual vocabulary without redistributing proprietary icons or font files. Use local/system fonts or approved project assets. Dark/light theme tokens should cover states, selection, validation severity and gizmo visibility; never communicate errors by color alone.

Inspect 1080p at 100%, 125% and 150% display scale and a smaller desktop viewport. Avoid clipped Apply/Cancel buttons, nested horizontal scrolling and default-open advanced tables. Allow expert users to open detailed tools without forcing them on beginners. Keyboard commands must be discoverable and avoid interfering with native editor commands.

## Source tools versus service tools

Landscape geometry edits remain in the source workflow. Nexus gizmos move authored place/feature offsets and supported interaction handles, not a hidden duplicate spline. A readonly source point in a batch is explicitly skipped, or the operation requires a reviewed source-edit mode; it is never silently modified.

Selecting in viewport/tree/details synchronizes selection and focuses the correct owner/handle. Array reorder does not change handle identity. Local/world modes, axis constraints and translation/rotation snapping preserve meaningful reference frames.

## Transaction lifecycle

Begin drag captures a baseline. Drag updates a preview without creating hundreds of undo steps. Cancel restores the baseline. Accept commits one named transaction with all affected dependencies. Revert discards an uncommitted draft; Undo reverses a committed transaction. These are distinct controls.

The implementation requires real engine undo/redo across hierarchy, config, profile, component, sub-point, reparent and batch edits. The browser prototypes intentionally omit production undo/redo and durable saves; that omission is not permission to omit them from V8.

Move/rotate are general. Scale is visible only when the selected schema supports corresponding dimensions and geometry/network recomputation. Batch previews list affected and skipped entities with reasons. Deleting or changing a shared object previews dependent highways/services.

## Lifecycle, diagnostics and budget

Mode/tab/map changes during interaction must release capture and avoid stale object bindings or fights with native spline visualizers. Selection remains stable or clears explicitly. Diagnostics use the same derived geometry/graph as compilation, not an independently approximated network.

Always-draw is an optional debug policy, not unlimited draw count. Apply culling, LOD and caps while prioritizing selected objects. Every issue identifies an entity/lane/port and offers Select; only safe reviewed fixes offer Fix, with a transaction and provenance.

## Acceptance anchors

R03/R04/R14/R19/R20. P01-T10/P01-T11/P01-T15; P03-T08/P03-T09; P04-T10–P04-T15; P05-T15.
