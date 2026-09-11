# NexusFramework V8 — Phase 1 and 2 source candidate

This folder contains the patched Unreal C++ project, not an HTML prototype. It targets UE 5.8, but the patched native build has not been verified in the delivery environment. Phase 1 and 2 have remaining implementation and acceptance work; Phases 3-5 are not implemented.

## Build and open

Close any editor instance using this project. From this folder, double-click `LAUNCH-EDITOR.bat`, which attempts a build before launch. To use a custom engine location:

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File .\Scripts\Launch-Editor.ps1 -EngineRoot "C:\Program Files\Epic Games\UE_5.8"
```

In Unreal, open **Tools > Nexus V8 Road Editor**, then choose **Demo > Build preview**. The demo is a synthetic 6-to-4-to-2 road fixture, not a live Landscape scan. Use a backed-up separate test map for real Landscape authoring checks.

`BUILD-EDITOR.bat` builds without launching. `RUN-CORE-TESTS.bat` runs the portable C++ tests and preview exporter. `RUN-UNREAL-TESTS.bat` runs eight supplied native automation tests after building. Each runner writes fresh logs under `Saved/Verification`. The Windows runners and the native editor remain unverified here.

Linux portable verification:

```bash
bash Scripts/test-portable.sh
```

## Included fixes

Removed the project override of the final `SEditorViewport::MakeViewportToolbar` method, including its definition. Replaced the unavailable `Misc/LexFromString.h` dependency with `Misc/DefaultValueHelper.h` and updated numeric parsing, retaining finite-value validation. No Unreal Engine installation files are changed.

## Status and evidence

- [Implementation status and remaining work](Docs/PHASE-STATUS.md)
- [Current verification and source provenance](Docs/VERIFICATION.md)
- [Engine acceptance checklist and all thirty scenarios](Docs/ENGINE-TEST-CHECKLIST.md)
- [Actual portable test log](Evidence/tests.log)

The retry's shared-core run passed 88 tests and 24,243 assertions. That run does not compile Slate/Unreal modules or prove engine acceptance.

## Repository layout and working branch

Continue all work on **gpt6**. The repository-root `AGENT-INSTRUCTIONS.md` is authoritative; do not create new phase/hotfix branches or change master without authorization.

All Source and Tests bytes match the previously supplied patched ZIP. The engine descriptor, config, CMake, BAT wrappers and six build/launch/test scripts are retained. Generated sample OBJ/BMP/JSON outputs, build caches, obsolete push helpers and ZIP-only manifests/audit reports are deliberately excluded from Git. The included exporter recreates previews under the portable run's Generated directory. Repository-specific documentation and fresh test logs replace historical package reports; the original five-phase planning documents remain available.

This project produces non-publishable previews. It does not yet provide complete runtime driving AI, junction/service editors or phase acceptance.
