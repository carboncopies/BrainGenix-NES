#! /bin/bash

# Create Build Directory
mkdir -p Build

# Build With CMAKE
cd Build
cmake ..
make