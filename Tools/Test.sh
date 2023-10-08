#!/bin/bash

echo "Entering Build Directory"
cd ../Build

echo "Running Tests"
ctest

# Return to Tools directory
cd ../Tools
