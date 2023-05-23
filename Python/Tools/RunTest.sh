#!/bin/bash

# Create And Enter Test Directory
cd ..
echo "Opening Python Interactive Shell In Test Env"
python3 -m venv TestEnv
source TestEnv/bin/activate
python3

