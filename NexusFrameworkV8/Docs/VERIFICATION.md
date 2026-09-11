# Source import verification — 2026-09-11

## Input and preservation

Input archive: `NexusFramework_V8_Phases_1_2_UE58_Fixed.zip`.
SHA-256: `d9d58b6e27c30dfc8347391f1d9e57357abea01b18feef659a1b1148c6595b75`.

The complete Source tree is `44e2413c648295f233dda0736dd1a074a57a8af2` and the complete Tests tree is `bf210c82592e7281ed311ba0ce989fc6daa6b46e`. These Git object hashes were calculated from the extracted package and matched the GitHub-created trees before commit. Source and test content was not rewritten during this push retry.

Repository baseline: `b0394d5573b6c92caf852093d98ea23fc2c422cd`. Working branch: `gpt6`. Existing root planning documents and `master` are outside the source import's mutation scope.

## Executed portable verification

GNU C++ 14.2.0, Debug, C++20, Linux. Commands executed against the extracted patched project:

```text
cmake -S /mnt/data/push_retry/patched/NexusFrameworkV8 -B /mnt/data/push_retry/core-build -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_STANDARD=20
cmake --build /mnt/data/push_retry/core-build -j 2
/mnt/data/push_retry/core-build/nexus_tests
```

**88 tests; 24,243 assertions; zero failures.** The actual configure, build and test stdout logs are in `../Evidence/configure.log`, `../Evidence/build.log`, and `../Evidence/tests.log`.

These tests compile the same shared C++ domain sources used by the project. They do not compile Unreal-specific modules. The preview exporter was built in this run; generated sample outputs can be recreated with the supplied portable runner.

## Not verified here

UHT/UBT/MSVC and Unreal linking; patched native editor execution; the eight native automation tests; the thirty full engine scenarios; Windows PowerShell/BAT execution; Windows-specific persistence; runtime traffic behavior. The earlier user's native build failed before this patch. A new native build is required.

## Repository packaging

Source, tests, engine descriptor, config and six build/launch/test scripts are retained. Large generated example meshes/textures and historical package-audit artifacts are not imported. The exporter recreates previews. The old ZIP-specific package manifest and audit scripts are not reused because their inventory includes those omitted artifacts. Fresh verification logs and repository-specific handoff documentation replace the old packaging reports. Original phase plans remain available.
