mkdir Build

cd Build
cmake ..
cmake --build . --target Lucifer-Example -j $1

cd ..
cd Binaries
./Lucifer-Example
cd ..