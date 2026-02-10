#!/bin/bash

# Enter Artifact/Binary Dir
cd ..
cd Binaries

# Add the vendored libraries to the Python path so the interpreter can find them.
export PYTHONPATH=$(pwd)/../venv/vendored_libs:$PYTHONPATH

# Activate the Python virtual environment
source ../venv/bin/activate

# Run the application
./BrainGenix-NES $1 $2 $3 $4
