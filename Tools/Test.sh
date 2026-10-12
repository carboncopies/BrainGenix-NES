#!/bin/bash
# Run the NES unit tests from the Build directory.
# Exits non-zero if any test fails (or if the tests cannot be run).
# Every test has a ctest TIMEOUT (60 s); a hang fails the run.
# Tests labelled known_bug (D8 in dev_frame) are excluded from the gate.

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="${BUILD_DIR:-$SCRIPT_DIR/../Build}"

echo "Entering Build Directory"
cd "$BUILD_DIR" || { echo "Build directory not found: $BUILD_DIR"; exit 2; }

echo "Running Tests"
# Per-test TIMEOUT is set in Source/Core/CMakeLists.txt; --timeout is a backstop for any test without one.
# A timed-out test is a failed test, so ctest (and this script) exits non-zero.
GTEST_COLOR=1 ctest --output-on-failure --timeout "${NES_TEST_TIMEOUT_SECONDS:-60}" -LE known_bug
RC=$?

echo "ctest exit code: $RC"

# Known bugs (suites named KnownBug*, label known_bug) are reported separately and never gate.
echo "Known bugs (not gating):"
ctest -N -L known_bug | tail -n +2
exit $RC
