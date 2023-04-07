cd ..
mkdir Build
cd Build
cmake ..
make -j
# cmake --build . --target test
make test