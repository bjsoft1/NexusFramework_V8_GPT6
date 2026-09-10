# V8 — twenty prototype review points

1. Use the 1920 × 1080 landscape layout as the primary editor reference.
2. Keep the viewport central with compact Hierarchy, Details, Activity, and Content Browser surfaces.
3. Keep dark/light modes, legible labels, small icons, keyboard access, and visible focus states.
4. Follow 60 distinct screens with Previous/Next, group navigation, and searchable screen IDs.
5. Keep Landscape authoritative for point/segment geometry; Nexus owns semantic authoring and bindings.
6. Review scans and reconciliation diffs; preserve authored metadata when source geometry changes.
7. Use stable GUIDs independently from display names and developer codes.
8. Use reusable mesh-config data assets with extensible entries, including service component meshes.
9. Resolve per entry in the order Highway → City → State → Root; show where every value came from.
10. Keep missing config assets transparent; clear overrides back to inheritance without copying parent values.
11. Keep points outside the mesh cascade and block unresolved required mesh entries.
12. Confirm highway-default plus segment-specific lane profiles before schema implementation.
13. Preserve lane count/direction/width, walking space, curbs, borders, and lane-transition semantics.
14. Keep crosswalk span distinct from depth; depth follows the requested sidewalk-plus-border rule.
15. Cover T-junctions, four-way turns, roundabouts, signal groups, and pedestrian conflict relationships.
16. Preserve complete bus-stop, shelter, base, indicator, waiting, boarding, and seat capabilities.
17. Preserve parking dimensions/gaps/bays/access and separate runtime occupancy/reservation state.
18. Edit three or more independent sub-points with a generic gizmo; add real undo/redo only in implementation.
19. Export valid highway/lane/service relationships for future drive, walk, lane-change, and service consumers.
20. Approve prototypes, finalize five phase plans, then build/test/review exactly one phase at a time.
