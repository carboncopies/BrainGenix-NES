#!/bin/bash

# Create And Enter Test Directory
cd ..
echo "Setting Up Venv Test Environment"
python3 -m venv TestEnv
source TestEnv/bin/activate


# Install NES Repo
echo "Installing NES Package"
pip3 install dist/*.whl

# Run Example Usage Code
cd Test
echo "Running Simple API Test"
python3 SimpleTest.py
echo "Running More Extensive API Test"
python3 ExampleUsage.py
