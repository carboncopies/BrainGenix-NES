#!/bin/bash

# Enter Artifact/Binary Dir
cd ..
cd Binaries

# Activate the Python virtual environment
source ../venv/bin/activate

# Run the application
cgdb --args ./BrainGenix-NES $1 $2 $3 $4
