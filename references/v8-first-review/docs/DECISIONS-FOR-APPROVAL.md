# V8 — decisions for prototype approval

**Status: design proposals, not approved implementation decisions.**

| Topic | Proposed V8 rule | Why it is visible now | Screens |
|---|---|---|---|
| Geometry ownership | Landscape owns coordinates/tangents/connectivity source; Nexus stores bindings and semantics. | Avoid a second conflicting spline model. | 04–10 |
| Geometry edits | Native Landscape tools create/move points; Nexus source views read/inspect them. Service gizmos edit authored offsets. | Distinguish geometry authoring from activation placement. | 08–09, 30–38 |
| Mesh resolution | Resolve each semantic entry Highway → City → State → Root. | A City shelter override must not shadow a Root road entry. | 14–18 |
| Null asset / entry | Both mean unassigned here; walk to parent. | A level need not copy an entire asset to override one component. | 15–18 |
| Deepest mesh scope | Highway. Points do not participate. | Preserve the explicitly stated boundary in V7 backlog B-02. | 17–18 |
| Activation mesh ownership | Catalog owns meshes; activation payloads own behavior, geometry, offsets, and slot keys. | Eliminate duplicate ownership between service payloads and shared config. | 28–36 |
| Lane-profile ownership | Highway default profile plus an explicit segment-profile reference where a road widens or narrows. | Prior backlog B-05 recorded this unresolved producer/schema decision. | 20–23 |
| Point features | Derived summaries of linked activation records; developer code remains separately authored. | Avoid editable flags that disagree with actual services. | 06, 09, 51 |
| Developer code | Stable GUID is identity; code is a human/developer helper. Uniqueness policy to finalize with schema approval. | Code cannot replace runtime identity. | 02, 51 |
| Crosswalk dimensions | Span crosses the carriageway; depth follows travel and equals sidewalk width + border width in the shown rule. | Avoid the prior width/depth ambiguity. | 22, 30 |
| Logical vs physical seats | Several logical interaction seats may share one physical mesh. | Capacity is not mesh count. | 34, 54 |
| Dynamic state | Runtime signals, agent state, seat reservations, and parking occupancy are not authoritative static authoring. | Pooled actor removal must not lose logical service state. | 55–56 |
| Undo/redo | Omitted in HTML; one Unreal-authoritative transaction per real committed action. | No competing browser/editor undo stack. | 11, 37–38, 58 |
| Legacy input | Inspect V0–V7 behavior and lessons; migrate only explicitly supported versioned assets with a non-destructive preview. | The legacy archive is not a production-tested V8 codebase. | 07 |

Review these decisions before finalizing phase tasks. A working browser fixture is not approval of the underlying schema.
