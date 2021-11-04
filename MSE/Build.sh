#! /bin/bash
mkdir -p Build
cd Build
cmake ..
make -j 24

cd ../Binaries
./BrainGenix-NES
cd ..
