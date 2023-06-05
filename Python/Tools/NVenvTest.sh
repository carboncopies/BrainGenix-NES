#!/bin/bash

# Setup And Enter Test Directory
cd ..
mkdir -p .Test
cp -r BrainGenix .Test
cp -r Test/* .Test
echo "" >> .Test/__init__.py
cd .Test

# Run Example Usage Code
echo "Running Simple API Test"
echo "---------------------------"
python3 SimpleTest.py
echo "---------------------------"
echo "Running More Extensive API Test"
echo "---------------------------"
python3 ExampleUsage.py
echo "---------------------------"