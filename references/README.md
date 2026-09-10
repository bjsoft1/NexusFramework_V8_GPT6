# Historical reference documents and provenance

Only the outer `phases/` folder contains the five current implementation plans. This reference area preserves older requirements, drafts, decisions, audits and results without rewriting their historical text. Older task/agent instructions and completion claims are not authority for current V8 work.

## Included

`v8-first-review/`: every Markdown/text document from the first review package, with JSON fixtures/results (22 files total).

`v8-1-review/`: every Markdown/text document from the simplified revision, with JSON fixtures/results (20 files total).

[Legacy V0–V7 documentation ZIP](Legacy-V0-V7-Documentation.zip): 612 original documentation/support files, preserving paths from the uploaded reference archive. Includes all original `.md`, `.txt` and `.html` documents, documentation CSS/JS, and documentation-folder JSON. The nested archive includes a file hash manifest. It is a reference copy, not a newly implemented V8 repository.

[Prototype references](../prototype-reference/README.md): the latest simplified and earlier 60-screen single-file HTML references, unchanged.

## Scope and link limitations

This is a documentation handoff, not a duplicate of the entire historical source repository. C++/C# source, binary/assets, font files, original large screenshot sets and complete historical test environments are not included. Source links inside copied historical text may require the original upload or prototype ZIP. Use the outer start page and current phase links for self-contained current documentation.

The original code archive contains reference trees V0 through V7; its filename does not establish a completed V8 implementation. Legacy documentation was preserved programmatically, not exhaustively re-audited for feature parity. Phase 1 requires that feature inventory before code is migrated.

## Source files preserved unchanged

| Input | Bytes | SHA-256 |
|---|---:|---|
| `NexusFramework_V8(1).zip` | 5,004,858 | `aa0363d9936f94bb75b7abd5b66f140dd1b4c415f0f49a61e12683e429d428ac` |
| `NexusFramework_V8_Prototype.zip` | 24,785,730 | `385c56b4264fc4371d5ac404c643bc32bbff64a6e4d00af42c64411b941e42f8` |
| `NexusFramework_V8_1_Simplified_Review.zip` | 2,524,187 | `2f6b18485bddefc46d0807c6f8eb2ca1f3dd4a5b59b4b171a87ce1b95d4a52e7` |
| `NexusFramework_V8_Review.html` | 259,723 | `a42e0de153a0b5dcf4c6448dedb7c70667fdae8576beb8523ca681bf356bca2c` |
| `NexusFramework_V8_1_Simple_Review.html` | 66,384 | `e7dec137d130b9547db1e4cb090c0d800b8b251975a3a3a5b33ee58974750591` |

Earlier research citations remain inside the preserved documents as historical attribution. No fresh engine/API compatibility research or engine test execution was done for this repackaging task. The current contracts consolidate the user's instructions and existing V8.1 proposals; pending choices are listed in the decision register.
