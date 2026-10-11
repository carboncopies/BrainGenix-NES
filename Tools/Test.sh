#!/bin/bash
# Run the NES unit tests from the Build directory.
# Exits non-zero if any test fails (or if the tests cannot be run).
# Tests labelled known_bug (D8 in dev_frame) are excluded from the gate.

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="${BUILD_DIR:-$SCRIPT_DIR/../Build}"

echo "Entering Build Directory"
cd "$BUILD_DIR" || { echo "Build directory not found: $BUILD_DIR"; exit 2; }

echo "Running Tests"
GTEST_COLOR=1 ctest --output-on-failure -LE known_bug
RC=$?

echo "ctest exit code: $RC"
exit $RC
