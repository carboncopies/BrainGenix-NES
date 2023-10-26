#!/bin/bash

echo "Entering Build Directory"
cd ../Build

echo "Running Tests"
GTEST_COLOR=1 ctest --output-on-failure

# Return to Tools directory
cd ../Tools
