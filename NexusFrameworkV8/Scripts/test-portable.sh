#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)"
RUN="$ROOT/Saved/Verification/Portable-$(date -u +%Y%m%d-%H%M%S)-$$"
BUILD="${BUILD_DIR:-$ROOT/Saved/PortableBuild}"
mkdir -p "$RUN"
cmake -S "$ROOT" -B "$BUILD" -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_STANDARD=20 \
  -DNEXUS_SANITIZE="${NEXUS_SANITIZE:-OFF}" 2>&1 | tee "$RUN/configure.log"
cmake --build "$BUILD" --parallel 2 2>&1 | tee "$RUN/build.log"
"$BUILD/nexus_tests" "$RUN/Results" 2>&1 | tee "$RUN/tests.log"
"$BUILD/nexus_preview" "$RUN/Generated" 2>&1 | tee "$RUN/export.log"
printf '\nPortable run finished: %s\nUnreal engine acceptance is NOT included.\n' "$RUN"
