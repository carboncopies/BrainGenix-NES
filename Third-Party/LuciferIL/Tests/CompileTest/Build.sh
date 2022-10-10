mkdir Build

cd Build
cmake ..
cmake --build . --target Lucifer-CompileTest -j $1

cd ..
cd Binaries
./Lucifer-CompileTest
cd ..