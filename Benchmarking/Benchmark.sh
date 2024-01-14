#!/bin/bash

echo "----------------------------"
echo "Running NES Benchmarks"
echo "This benchmark simply measures the time to create and render a small tissue sample."
echo "Please compare against your local machine times, as this is very much based on your hardware speed."
echo "----------------------------"

cd Renderer
./RenderBenchmarks.py
cd ..

