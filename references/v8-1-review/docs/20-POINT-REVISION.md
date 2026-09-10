# V8.1 — twenty review points

**Status: prototype revision for review. No production phase has started.**

1. Use one editor with six task workspaces, not sixty mandatory navigation steps.
2. Keep World/Root → State → City → Highway → Highway Point → Place as the authoring tree.
3. Treat World and Root as one level, never two nested configuration levels.
4. Keep points as anchors and segments as the curves connecting those anchors.
5. Keep Landscape geometry authoritative; bind each connected segment end and its tangent explicitly.
6. Show a point's before/after lane profiles without duplicating contradictory lane counts.
7. Store real adapter definitions and directional lane links for 6 → 4 → 2 and other transitions.
8. On two-way roads, distinguish forward merges from reverse-direction splits.
9. Make crosswalk, junction, roundabout and signal summaries derive from real component references.
10. Store each junction/roundabout once; all highway approach points reference it.
11. Describe junction type, approach profiles, legal turns, signal groups and conflict zones.
12. Describe roundabout type, circulating lanes, direction, entry ports and legal exit connections.
13. Attach parking, bus stops, hospitals, city parks and petrol pumps using local location/rotation offsets.
14. Give services real vehicle/pedestrian access links and independent interaction points, not only a mesh position.
15. Preserve Highway → City → State → Root mesh fallback, with compatible type/profile variants.
16. Compile mesh output and AI lane/walk networks from the same validated source revision.
17. Expose lane successors, legal changes, upcoming features, reachable services and unique roundabout queries.
18. Stop or replan when no legal route exists; never invent a straight-line road fallback.
19. Keep advanced schemas, IDs, link matrices and debug layers behind Advanced without deleting capabilities.
20. Approve this revised prototype, then finalize five phase plans; implement, test and approve one phase at a time.

Real transactional undo/redo belongs in implementation. The browser prototype deliberately omits it.
