# Phase gate report template

Copy this file for a phase when its implementation starts. Do not edit a prior approved report to conceal new failures.

## Identity and baseline

| Field | Value |
|---|---|
| Phase | NOT RECORDED |
| Plan/package revision | NOT RECORDED |
| Prototype/plan approval reference | NOT RECORDED |
| Prior phase approval reference | NOT RECORDED |
| Repository commit / dirty diff | NOT RECORDED |
| Engine version / build | NOT RECORDED |
| Toolchain / OS / hardware | NOT RECORDED |
| Schema/compiler/catalog/source revisions | NOT RECORDED |
| Fixture IDs / seed / configuration | NOT RECORDED |

## Task and test evidence

| Task/test ID | Exact command or reproducible procedure | Expected | Observed | Result | Evidence path |
|---|---|---|---|---|---|
| NOT RECORDED | NOT EXECUTED | From current phase plan | NOT OBSERVED | NOT_EXECUTED | None |

Result vocabulary: NOT_EXECUTED, PASS, FAIL, BLOCKED. A planned command, copied historical report or simulated browser action is not PASS for an engine test. Record duration only after execution; include full error logs for failures.

## Required evidence groups

Build and launch; automated tests; manual UI/workflow checks; source/authoring preservation and save/reopen; negative/fault cases; screenshots/recordings showing selected IDs; changed files and requirement traceability; relevant performance measurements.

## Data and regression review

Record before/after source hashes, intended authoring changes, backup locations, interruption/recovery results, generated-asset ownership, all prior tests rerun, unresolved failures and impact. For geometry/navigation tests include agent dimensions, speed, tolerances, sampler/sweep method and trajectory evidence.

## Review decision

Implementation: NOT_STARTED  
Automated engine tests: NOT_EXECUTED  
Manual engine verification: NOT_EXECUTED  
Critical failures: NOT_ASSESSED  
Reviewer decision: PENDING  
Approved next action: NONE

Record reviewer, timestamp, exact scope and explicit approval message/reference. Allowed decisions are APPROVED, CHANGES_REQUIRED or REJECTED. CHANGES_REQUIRED is not permission to implement the next phase. Phase 5 additionally requires final release approval.

No automatic continuation based on elapsed time, test count or agent self-approval. Only an explicit user/reviewer decision opens the next implementation phase.
