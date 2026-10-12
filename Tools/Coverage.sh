#!/bin/bash
# Build NES with gcov instrumentation, run the test gate, and write a coverage report.
#
#   Tools/Coverage.sh [jobs]
#
# Build dir: Build-Coverage/ (Debug, NES_COVERAGE=ON). Reports: Build-Coverage/coverage/
#   coverage.xml (Cobertura), index.html (HTML), summary.txt.
# Gate: `ctest -LE known_bug` (known_bug tests are run and listed separately, not gating).
# Exit: non-zero if the build fails, any gate test fails, or gcovr fails.
# Requires gcovr (>= 5): on PATH, or at .cov-venv/bin/gcovr (python3 -m venv .cov-venv && .cov-venv/bin/pip install gcovr).
# Env: BUILD_DIR overrides the build dir. Set COVERAGE_NO_BUILD=1 to skip configure/build.

set -u
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
JOBS="${1:-$(nproc)}"
BUILD_DIR="${BUILD_DIR:-$ROOT/Build-Coverage}"
OUT_DIR="$BUILD_DIR/coverage"

GCOVR="$(command -v gcovr || true)"
[ -z "$GCOVR" ] && [ -x "$ROOT/.cov-venv/bin/gcovr" ] && GCOVR="$ROOT/.cov-venv/bin/gcovr"
if [ -z "$GCOVR" ]; then
    echo "[COVERAGE] gcovr not found. Install: python3 -m venv .cov-venv && .cov-venv/bin/pip install gcovr" >&2
    exit 2
fi
echo "[COVERAGE] $($GCOVR --version | head -1), jobs=$JOBS, build dir=$BUILD_DIR"

START=$(date +%s)
if [ "${COVERAGE_NO_BUILD:-0}" != "1" ]; then
    mkdir -p "$BUILD_DIR"
    cmake -S "$ROOT" -B "$BUILD_DIR" -D CMAKE_BUILD_TYPE=Debug -D NES_COVERAGE=ON || { echo "[COVERAGE] configure failed" >&2; exit 2; }
    cmake --build "$BUILD_DIR" -j "$JOBS" || { echo "[COVERAGE] build failed" >&2; exit 2; }
fi
BUILD_DONE=$(date +%s)
echo "[COVERAGE] build wall time: $((BUILD_DONE - START)) s"

# Zero counters from any earlier run so the report reflects only this one.
find "$BUILD_DIR" -name '*.gcda' -delete

cd "$BUILD_DIR" || exit 2
# Per-test TIMEOUT comes from Source/Core/CMakeLists.txt; --timeout is a backstop. A timeout is a failure.
GTEST_COLOR=1 ctest --output-on-failure --timeout "${NES_TEST_TIMEOUT_SECONDS:-60}" -LE known_bug
TEST_RC=$?
echo "[COVERAGE] gate ctest exit code: $TEST_RC"

echo "[COVERAGE] known_bug tests (not gating):"
ctest -L known_bug --output-on-failure --timeout "${NES_TEST_TIMEOUT_SECONDS:-60}" 2>&1 | tail -n 15

cd "$ROOT" || exit 2
mkdir -p "$OUT_DIR"
"$GCOVR" --config "$ROOT/gcovr.cfg" --object-directory "$BUILD_DIR" \
    --gcov-ignore-parse-errors=negative_hits.warn_once_per_file \
    --txt "$OUT_DIR/summary.txt" --print-summary \
    --xml "$OUT_DIR/coverage.xml" --xml-pretty \
    --html-details "$OUT_DIR/index.html" \
    -j "$JOBS"
GCOVR_RC=$?
echo "[COVERAGE] gcovr exit code: $GCOVR_RC"
echo "[COVERAGE] reports: $OUT_DIR/{summary.txt,coverage.xml,index.html}"
echo "[COVERAGE] total wall time: $(( $(date +%s) - START )) s"

[ $TEST_RC -ne 0 ] && exit $TEST_RC
[ $GCOVR_RC -ne 0 ] && exit $GCOVR_RC
exit 0
